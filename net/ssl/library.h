#ifndef NET_SSL_LIBRARY_H
#define NET_SSL_LIBRARY_H

#include "net/internal/ssl/openssl.h"
#include "net/ssl/version.h"

namespace net {
  namespace ssl {
    typedef internal::ssl::filetype filetype;
    typedef internal::ssl::thread_support thread_support;
    typedef internal::ssl::logger logger;

    class library {
      public:
        // Constructor.
        library();

        // Destructor.
        ~library();

        // Initialize.
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
        void set_logger(logger l, void* d);

      private:
        bool _M_initialized;
    };

    inline library::library()
      : _M_initialized(false)
    {
    }

    inline library::~library()
    {
      cleanup();
    }

    inline bool library::init(version v, thread_support threads)
    {
      return (_M_initialized = internal::ssl::init(v, threads));
    }

    inline void library::cleanup()
    {
      if (_M_initialized) {
        internal::ssl::cleanup();
        thread_cleanup();

        _M_initialized = false;
      }
    }

    inline void library::thread_cleanup()
    {
      internal::ssl::thread_cleanup();
    }

    inline void library::disallow_version(version v)
    {
      internal::ssl::disallow_version(v);
    }

    inline bool library::load_certificate(const char* filename)
    {
      return internal::ssl::load_certificate(filename);
    }

    inline bool library::load_private_key(const char* filename, filetype type)
    {
      return internal::ssl::load_private_key(filename, type);
    }

    inline bool library::set_cipher_list(const char* cipher_list)
    {
      return internal::ssl::set_cipher_list(cipher_list);
    }

    inline void library::set_logger(logger l, void* d)
    {
      internal::ssl::set_logger(l, d);
    }
  }
}

#endif // NET_SSL_LIBRARY_H
