#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <openssl/engine.h>
#include <openssl/conf.h>
#include <openssl/err.h>
#include <openssl/opensslv.h>
#include "net/internal/ssl/openssl.h"
#include "net/internal/socket/socket.h"

namespace net {
  namespace internal {
    namespace ssl {
      static SSL_CTX* ctx = nullptr;

      static pthread_mutex_t* locks = nullptr;
      static size_t nlocks = 0;

      static logger log = nullptr;
      static void* data = nullptr;

      static inline void locking_function(int mode,
                                          int n,
                                          const char* file,
                                          int line)
      {
        if (mode & CRYPTO_LOCK) {
          pthread_mutex_lock(&locks[n]);
        } else {
          pthread_mutex_unlock(&locks[n]);
        }
      }

      static void ssl_error(const char* text)
      {
        static constexpr const size_t err_max_len = 1024;

        // If a logger has been set...
        if (log) {
          char errmsg[err_max_len + 1];

          do {
            unsigned long err;
            if ((err = ERR_get_error()) != 0) {
              ERR_error_string_n(err, errmsg, sizeof(errmsg));

              log(text, errmsg, data);
            } else {
              break;
            }
          } while (true);
        }
      }

      bool init(version v, thread_support threads)
      {
#if (OPENSSL_VERSION_NUMBER >= 0x10100000L) && !defined(LIBRESSL_VERSION_NUMBER)
        if (OPENSSL_init_ssl(0, nullptr) != 1) {
          ssl_error("OPENSSL_init_ssl() failed");
          return false;
        }
#else
        // Register the available SSL/TLS ciphers and digests.
        SSL_library_init();

        // Register the error strings.
        SSL_load_error_strings();

        // Add all algorithms.
        OpenSSL_add_all_algorithms();
#endif

        const SSL_METHOD* method;
        switch (v) {
#if !defined(OPENSSL_NO_SSL3_METHOD)
          case version::SSLv3:
            method = SSLv3_method();
            break;
#endif // !defined(OPENSSL_NO_SSL3_METHOD)
          case version::TLSv1:
            method = TLSv1_method();
            break;
          case version::TLSv1_1:
            method = TLSv1_1_method();
            break;
          case version::TLSv1_2:
            method = TLSv1_2_method();
            break;
          case version::SSLv23:
            method = SSLv23_method();
            break;
          case version::DTLSv1:
            method = DTLSv1_method();
            break;
#if defined(SSL_OP_NO_DTLSv1_2)
          case version::DTLSv1_2:
            method = DTLSv1_2_method();
            break;
#endif // defined(SSL_OP_NO_DTLSv1_2)
          default:
            method = SSLv23_method();
        }

        if (method) {
          // Create SSL_CTX object.
          if ((ctx = SSL_CTX_new(method)) != nullptr) {
            // Read ahead as many bytes as possible.
            SSL_CTX_set_read_ahead(ctx, 1);

            if (threads == thread_support::enabled) {
              // Get the number of required locks.
              nlocks = CRYPTO_num_locks();

              if ((locks = static_cast<pthread_mutex_t*>(
                             malloc(nlocks * sizeof(pthread_mutex_t))
                           )) != nullptr) {
                // Initialize mutexes.
                for (size_t i = 0; i < nlocks; i++) {
                  if (pthread_mutex_init(&locks[i], nullptr) != 0) {
                    nlocks = i;

                    cleanup();
                    return false;
                  }
                }

                // Set locking callback.
                CRYPTO_set_locking_callback(locking_function);

                return true;
              }
            } else {
              // Without thread support.
              return true;
            }
          } else {
            ssl_error("SSL_CTX_new() failed");
          }
        }

        cleanup();

        return false;
      }

      void cleanup()
      {
        if (ctx) {
          SSL_CTX_free(ctx);
          ctx = nullptr;
        }

        // https://wiki.openssl.org/index.php/Library_Initialization#Cleanup
        CRYPTO_set_locking_callback(nullptr);

        ENGINE_cleanup();

        CONF_modules_unload(1);

        EVP_cleanup();

        ERR_free_strings();

        if (locks) {
          for (size_t i = 0; i < nlocks; i++) {
            pthread_mutex_destroy(&locks[i]);
          }

          free(locks);
          locks = nullptr;
        }

        nlocks = 0;
      }

      void thread_cleanup()
      {
        CRYPTO_cleanup_all_ex_data();

#if OPENSSL_VERSION_NUMBER < 0x10100000L
        ERR_remove_thread_state(nullptr);
#endif
      }

      void disallow_version(version v)
      {
        switch (v) {
#if !defined(OPENSSL_NO_SSL3_METHOD)
          case version::SSLv3:
            SSL_CTX_clear_options(ctx, SSL_OP_NO_SSLv3);
            break;
#endif // !defined(OPENSSL_NO_SSL3_METHOD)
          case version::TLSv1:
            SSL_CTX_clear_options(ctx, SSL_OP_NO_TLSv1);
            break;
          case version::TLSv1_1:
            SSL_CTX_clear_options(ctx, SSL_OP_NO_TLSv1_1);
            break;
          case version::TLSv1_2:
            SSL_CTX_clear_options(ctx, SSL_OP_NO_TLSv1_2);
            break;
#if defined(SSL_OP_NO_TLSv1_3)
          case version::TLSv1_3:
            SSL_CTX_clear_options(ctx, SSL_OP_NO_TLSv1_3);
            break;
#endif // defined(SSL_OP_NO_TLSv1_3)
#if defined(SSL_OP_NO_DTLSv1)
          case version::DTLSv1:
            SSL_CTX_clear_options(ctx, SSL_OP_NO_DTLSv1);
            break;
#endif // defined(SSL_OP_NO_DTLSv1)
#if defined(SSL_OP_NO_DTLSv1_2)
          case version::DTLSv1_2:
            SSL_CTX_clear_options(ctx, SSL_OP_NO_DTLSv1_2);
            break;
#endif // defined(SSL_OP_NO_DTLSv1_2)
          default:
            ;
        }
      }

      bool load_certificate(const char* filename)
      {
        // Clear the error queue.
        ERR_clear_error();

        if (SSL_CTX_use_certificate_chain_file(ctx, filename) == 1) {
          return true;
        }

        ssl_error("SSL_CTX_use_certificate_chain_file() failed");

        return false;
      }

      bool load_private_key(const char* filename, filetype type)
      {
        // Clear the error queue.
        ERR_clear_error();

        if (SSL_CTX_use_PrivateKey_file(ctx,
                                        filename,
                                        static_cast<int>(type)) == 1) {
          return true;
        }

        ssl_error("SSL_CTX_use_PrivateKey_file() failed");

        return false;
      }

      bool set_cipher_list(const char* cipher_list)
      {
        // Clear the error queue.
        ERR_clear_error();

        if (SSL_CTX_set_cipher_list(ctx, cipher_list) == 1) {
          return true;
        }

        ssl_error("SSL_CTX_set_cipher_list() failed");

        return false;
      }

      void set_logger(logger l, void* d)
      {
        log = l;
        data = d;
      }

      namespace socket {
        SSL* create(int fd, mode m)
        {
          SSL* ssl;
          if ((ssl = SSL_new(ctx)) != nullptr) {
            if (SSL_set_fd(ssl, fd) == 1) {
              if (m == mode::client) {
                SSL_set_connect_state(ssl);
              } else {
                SSL_set_accept_state(ssl);
              }

              return ssl;
            } else {
              ssl_error("SSL_set_fd() failed");
            }

            SSL_free(ssl);
          } else {
            ssl_error("SSL_new() failed");
          }

          return nullptr;
        }

        void destroy(SSL* ssl)
        {
          SSL_free(ssl);
        }

        bool handshake(SSL* ssl, bool& readable, bool& writable)
        {
          do {
            // Reset errno.
            errno = 0;

            // Clear the error queue.
            ERR_clear_error();

            // Perform TLS/SSL handshake.
            int ret;
            if ((ret = SSL_do_handshake(ssl)) == 1) {
              return true;
            }

            switch (SSL_get_error(ssl, ret)) {
              case SSL_ERROR_WANT_READ:
                readable = false;
                errno = EAGAIN;

                return false;
              case SSL_ERROR_WANT_WRITE:
                writable = false;
                errno = EAGAIN;

                return false;
              case SSL_ERROR_ZERO_RETURN:
                // The TLS/SSL connection has been closed.
                errno = ECONNRESET;
                return false;
              case SSL_ERROR_SYSCALL:
                if ((ret < 0) && (errno == EINTR)) {
                  continue;
                }

                errno = ECONNRESET;
                return false;
              case SSL_ERROR_SSL:
                ssl_error("SSL_do_handshake() failed");

                errno = ECONNRESET;
                return false;
            }
          } while (true);
        }

        bool handshake(SSL* ssl, int timeout)
        {
          // Get file descriptor.
          int fd = SSL_get_fd(ssl);

          do {
            bool readable = true;
            bool writable = true;
            if (handshake(ssl, readable, writable)) {
              return true;
            } else if (errno == EAGAIN) {
              if (!readable) {
                if (!net::internal::socket::wait_readable(fd, timeout)) {
                  return false;
                }
              } else {
                if (!net::internal::socket::wait_writable(fd, timeout)) {
                  return false;
                }
              }
            } else {
              return false;
            }
          } while (true);
        }

        static bool shutdown(SSL* ssl, bool& readable, bool& writable)
        {
          do {
            // Reset errno.
            errno = 0;

            // Clear the error queue.
            ERR_clear_error();

            // Shutdown TLS/SSL connection.
            int ret;
            switch (ret = SSL_shutdown(ssl)) {
              case 1:
                return true;
              case 0:
                continue;
              default:
                switch (SSL_get_error(ssl, ret)) {
                  case SSL_ERROR_WANT_READ:
                    readable = false;
                    errno = EAGAIN;

                    return false;
                  case SSL_ERROR_WANT_WRITE:
                    writable = false;
                    errno = EAGAIN;

                    return false;
                  case SSL_ERROR_ZERO_RETURN:
                    // The TLS/SSL connection has been closed.
                    errno = ECONNRESET;
                    return false;
                  case SSL_ERROR_SYSCALL:
                    if ((ret < 0) && (errno == EINTR)) {
                      continue;
                    }

                    errno = ECONNRESET;
                    return false;
                  case SSL_ERROR_SSL:
                    ssl_error("SSL_shutdown() failed");

                    errno = ECONNRESET;
                    return false;
                }
            }
          } while (true);
        }

        bool shutdown(SSL* ssl,
                      shutdown_how how,
                      bool& readable,
                      bool& writable)
        {
          if (how == shutdown_how::unidirectional) {
            SSL_set_shutdown(ssl,
                             SSL_get_shutdown(ssl) | SSL_RECEIVED_SHUTDOWN);
          }

          return shutdown(ssl, readable, writable);
        }

        bool shutdown(SSL* ssl, shutdown_how how, int timeout)
        {
          if (how == shutdown_how::unidirectional) {
            SSL_set_shutdown(ssl,
                             SSL_get_shutdown(ssl) | SSL_RECEIVED_SHUTDOWN);
          }

          // Get file descriptor.
          int fd = SSL_get_fd(ssl);

          do {
            bool readable = true;
            bool writable = true;
            if (shutdown(ssl, readable, writable)) {
              return true;
            } else if (errno == EAGAIN) {
              if (!readable) {
                if (!net::internal::socket::wait_readable(fd, timeout)) {
                  return false;
                }
              } else {
                if (!net::internal::socket::wait_writable(fd, timeout)) {
                  return false;
                }
              }
            } else {
              return false;
            }
          } while (true);
        }

        ssize_t recv(SSL* ssl,
                     void* buf,
                     size_t len,
                     bool& readable,
                     bool& writable)
        {
          do {
            // Reset errno.
            errno = 0;

            // Clear the error queue.
            ERR_clear_error();

            // Read.
            int ret;
            if ((ret = SSL_read(ssl, buf, len)) > 0) {
              return ret;
            }

            switch (SSL_get_error(ssl, ret)) {
              case SSL_ERROR_WANT_READ:
                readable = false;
                errno = EAGAIN;

                return -1;
              case SSL_ERROR_WANT_WRITE:
                writable = false;
                errno = EAGAIN;

                return -1;
              case SSL_ERROR_ZERO_RETURN:
                // The TLS/SSL connection has been closed.
                return 0;
              case SSL_ERROR_SYSCALL:
                if ((ret < 0) && (errno == EINTR)) {
                  continue;
                }

                errno = ECONNRESET;
                return -1;
              case SSL_ERROR_SSL:
                ssl_error("SSL_read() failed");

                errno = ECONNRESET;
                return -1;
            }
          } while (true);
        }

        ssize_t recv(SSL* ssl, void* buf, size_t len, int timeout)
        {
          // Get file descriptor.
          int fd = SSL_get_fd(ssl);

          do {
            bool readable = true;
            bool writable = true;
            ssize_t ret;
            if ((ret = recv(ssl, buf, len, readable, writable)) >= 0) {
              return ret;
            } else if (errno == EAGAIN) {
              if (!readable) {
                if (!net::internal::socket::wait_readable(fd, timeout)) {
                  return -1;
                }
              } else {
                if (!net::internal::socket::wait_writable(fd, timeout)) {
                  return -1;
                }
              }
            } else {
              return -1;
            }
          } while (true);
        }

        ssize_t send(SSL* ssl,
                     const void* buf,
                     size_t len,
                     bool& readable,
                     bool& writable)
        {
          do {
            // Reset errno.
            errno = 0;

            // Clear the error queue.
            ERR_clear_error();

            // Write.
            int ret;
            if ((ret = SSL_write(ssl, buf, len)) > 0) {
              return ret;
            }

            switch (SSL_get_error(ssl, ret)) {
              case SSL_ERROR_WANT_READ:
                readable = false;
                errno = EAGAIN;

                return -1;
              case SSL_ERROR_WANT_WRITE:
                writable = false;
                errno = EAGAIN;

                return -1;
              case SSL_ERROR_ZERO_RETURN:
                // The TLS/SSL connection has been closed.
                errno = ECONNRESET;
                return -1;
              case SSL_ERROR_SYSCALL:
                if ((ret < 0) && (errno == EINTR)) {
                  continue;
                }

                errno = ECONNRESET;
                return -1;
              case SSL_ERROR_SSL:
                ssl_error("SSL_write() failed");

                errno = ECONNRESET;
                return -1;
            }
          } while (true);
        }

        bool send(SSL* ssl, const void* buf, size_t len, int timeout)
        {
          // Get file descriptor.
          int fd = SSL_get_fd(ssl);

          do {
            bool readable = true;
            bool writable = true;
            ssize_t ret;
            if ((ret = send(ssl, buf, len, readable, writable)) > 0) {
              return true;
            } else if (errno == EAGAIN) {
              if (!readable) {
                if (!net::internal::socket::wait_readable(fd, timeout)) {
                  return false;
                }
              } else {
                if (!net::internal::socket::wait_writable(fd, timeout)) {
                  return false;
                }
              }
            } else {
              return false;
            }
          } while (true);
        }
      }
    }
  }
}
