#ifndef NET_INTERNAL_SSL_OPENSSL_H
#define NET_INTERNAL_SSL_OPENSSL_H

#include <openssl/ssl.h>
#include "net/internal/ssl/version.h"

namespace net {
  namespace internal {
    namespace ssl {
      enum class filetype {
        pem = SSL_FILETYPE_PEM,
        asn1 = SSL_FILETYPE_ASN1
      };

      // Initialize OpenSSL.
      enum class thread_support {
        enabled,
        disabled
      };

      bool init(version v, thread_support threads);

      // Cleanup OpenSSL.
      void cleanup();

      // Thread cleanup.
      void thread_cleanup();

      // Disallow a specific TLS/SSL version.
      void disallow_version(version v);

      // Load certificate.
      bool load_certificate(const char* filename);

      // Load private key.
      bool load_private_key(const char* filename,
                            filetype type = filetype::pem);

      // Set the list of available ciphers.
      bool set_cipher_list(const char* cipher_list);

      // Set logger.
      typedef void (*logger)(const char* text,
                             const char* errmsg,
                             void* data);

      void set_logger(logger l, void* d);

      namespace socket {
        enum class mode {
          client,
          server
        };

        // Create SSL structure.
        SSL* create(int fd, mode m);

        // Destroy SSL structure.
        void destroy(SSL* ssl);

        // Perform handshake.
        bool handshake(SSL* ssl, bool& readable, bool& writable);
        bool handshake(SSL* ssl, int timeout);

        // Shutdown.
        enum class shutdown_how {
          unidirectional,
          bidirectional
        };

        bool shutdown(SSL* ssl,
                      shutdown_how how,
                      bool& readable,
                      bool& writable);

        bool shutdown(SSL* ssl, shutdown_how how, int timeout);

        // Receive.
        ssize_t recv(SSL* ssl,
                     void* buf,
                     size_t len,
                     bool& readable,
                     bool& writable);

        ssize_t recv(SSL* ssl, void* buf, size_t len, int timeout);

        // Send.
        ssize_t send(SSL* ssl,
                     const void* buf,
                     size_t len,
                     bool& readable,
                     bool& writable);

        bool send(SSL* ssl, const void* buf, size_t len, int timeout);
      }
    }
  }
}

#endif // NET_INTERNAL_SSL_OPENSSL_H
