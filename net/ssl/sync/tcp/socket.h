#ifndef NET_SSL_SYNC_TCP_SOCKET_H
#define NET_SSL_SYNC_TCP_SOCKET_H

#include "net/socket.h"
#include "net/ssl/socket.h"

namespace net {
  namespace ssl {
    namespace sync {
      namespace tcp {
        class socket {
          public:
            // Constructor.
            socket(net::socket& s);
            socket(net::socket::handle_t h);

            // Destructor.
            ~socket();

            // Close socket.
            void close();

            // Clear socket.
            void clear();

            // Perform handshake.
            bool handshake(ssl::socket::mode m, int timeout);

            // Shutdown socket.
            bool shutdown(ssl::socket::shutdown_how how, int timeout);

            // Receive.
            ssize_t recv(void* buf, size_t len, int timeout);

            // Send.
            bool send(const void* buf, size_t len, int timeout);

            // Get handle.
            net::socket::handle_t handle() const;

          private:
            net::socket _M_socket;
            SSL* _M_ssl;
        };

        inline socket::socket(net::socket& s)
          : _M_socket(s.handle()),
            _M_ssl(nullptr)
        {
          s.clear();
        }

        inline socket::socket(net::socket::handle_t h)
          : _M_socket(h),
            _M_ssl(nullptr)
        {
        }

        inline socket::~socket()
        {
          close();
        }

        inline void socket::close()
        {
          if (_M_ssl) {
            internal::ssl::socket::destroy(_M_ssl);
            _M_ssl = nullptr;
          }

          _M_socket.close();
        }

        inline void socket::clear()
        {
          if (_M_ssl) {
            internal::ssl::socket::destroy(_M_ssl);
            _M_ssl = nullptr;
          }

          _M_socket.clear();
        }

        inline bool socket::handshake(ssl::socket::mode m, int timeout)
        {
          return (((_M_ssl = internal::ssl::socket::create(_M_socket.handle(),
                                                           m)) != nullptr) &&
                  (internal::ssl::socket::handshake(_M_ssl, timeout)));
        }

        inline bool socket::shutdown(ssl::socket::shutdown_how how, int timeout)
        {
          return internal::ssl::socket::shutdown(_M_ssl, how, timeout);
        }

        inline ssize_t socket::recv(void* buf, size_t len, int timeout)
        {
          return internal::ssl::socket::recv(_M_ssl, buf, len, timeout);
        }

        inline bool socket::send(const void* buf, size_t len, int timeout)
        {
          return internal::ssl::socket::send(_M_ssl, buf, len, timeout);
        }

        inline net::socket::handle_t socket::handle() const
        {
          return _M_socket.handle();
        }
      }
    }
  }
}

#endif // NET_SSL_SYNC_TCP_SOCKET_H
