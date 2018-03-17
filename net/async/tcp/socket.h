#ifndef NET_ASYNC_TCP_SOCKET_H
#define NET_ASYNC_TCP_SOCKET_H

#include "net/async/socket.h"

namespace net {
  namespace async {
    namespace tcp {
      class socket : public net::async::socket {
        public:
          // Create socket.
          using net::async::socket::create;
          bool create(domain d, type t) = delete;

          // Connect.
          bool connect(const address& addr);
          bool connect(const address::ipv4& addr);
          bool connect(const address::ipv6& addr);
          bool connect(const address::local& addr);

          // Bind.
          using net::async::socket::bind;
          bool bind(const address& addr) = delete;
          bool bind(const address::ipv4& addr) = delete;
          bool bind(const address::ipv6& addr) = delete;
          bool bind(const address::local& addr) = delete;

          // Listen.
          using net::async::socket::listen;
          bool listen() = delete;
          bool listen(const address& addr);
          bool listen(const address::ipv4& addr);
          bool listen(const address::ipv6& addr);
          bool listen(const address::local& addr);

          // Receive from.
          using net::async::socket::recvfrom;
          ssize_t recvfrom(void* buf, size_t len, address& addr) = delete;
          ssize_t recvfrom(void* buf, size_t len) = delete;

          // Send to.
          using net::async::socket::sendto;
          ssize_t sendto(const void* buf,
                         size_t len,
                         const address& addr) = delete;

          ssize_t sendto(const void* buf,
                         size_t len,
                         const address::ipv4& addr) = delete;

          ssize_t sendto(const void* buf,
                         size_t len,
                         const address::ipv6& addr) = delete;

          ssize_t sendto(const void* buf,
                         size_t len,
                         const address::local& addr) = delete;

          ssize_t sendto(const void* buf, size_t len) = delete;
      };

      inline bool socket::connect(const address& addr)
      {
        return ((net::socket::create(static_cast<socket::domain>(addr.family()),
                                     socket::type::stream)) &&
                (async::socket::connect(addr)));
      }

      inline bool socket::connect(const address::ipv4& addr)
      {
        return ((net::socket::create(static_cast<socket::domain>(addr.family()),
                                     socket::type::stream)) &&
                (async::socket::connect(addr)));
      }

      inline bool socket::connect(const address::ipv6& addr)
      {
        return ((net::socket::create(static_cast<socket::domain>(addr.family()),
                                     socket::type::stream)) &&
                (async::socket::connect(addr)));
      }

      inline bool socket::connect(const address::local& addr)
      {
        return ((net::socket::create(static_cast<socket::domain>(addr.family()),
                                     socket::type::stream)) &&
                (async::socket::connect(addr)));
      }

      inline bool socket::listen(const address& addr)
      {
        return ((net::socket::create(static_cast<socket::domain>(addr.family()),
                                     socket::type::stream)) &&
                (net::socket::bind(addr)) &&
                (net::socket::listen()));
      }

      inline bool socket::listen(const address::ipv4& addr)
      {
        return ((net::socket::create(static_cast<socket::domain>(addr.family()),
                                     socket::type::stream)) &&
                (net::socket::bind(addr)) &&
                (net::socket::listen()));
      }

      inline bool socket::listen(const address::ipv6& addr)
      {
        return ((net::socket::create(static_cast<socket::domain>(addr.family()),
                                     socket::type::stream)) &&
                (net::socket::bind(addr)) &&
                (net::socket::listen()));
      }

      inline bool socket::listen(const address::local& addr)
      {
        return ((net::socket::create(static_cast<socket::domain>(addr.family()),
                                     socket::type::stream)) &&
                (net::socket::bind(addr)) &&
                (net::socket::listen()));
      }
    }
  }
}

#endif // NET_ASYNC_TCP_SOCKET_H
