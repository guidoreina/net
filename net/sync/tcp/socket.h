#ifndef NET_SYNC_TCP_SOCKET_H
#define NET_SYNC_TCP_SOCKET_H

#include "net/sync/socket.h"

namespace net {
  namespace sync {
    namespace tcp {
      class socket : public net::sync::socket {
        public:
          // Create socket.
          using net::sync::socket::create;
          bool create(domain d, type t) = delete;

          // Connect.
          bool connect(const address& addr, int timeout);
          bool connect(const address::ipv4& addr, int timeout);
          bool connect(const address::ipv6& addr, int timeout);
          bool connect(const address::local& addr, int timeout);

          // Bind.
          using net::sync::socket::bind;
          bool bind(const address& addr) = delete;
          bool bind(const address::ipv4& addr) = delete;
          bool bind(const address::ipv6& addr) = delete;
          bool bind(const address::local& addr) = delete;

          // Listen.
          using net::sync::socket::listen;
          bool listen() = delete;
          bool listen(const address& addr);
          bool listen(const address::ipv4& addr);
          bool listen(const address::ipv6& addr);
          bool listen(const address::local& addr);

          // Receive from.
          using net::sync::socket::recvfrom;
          ssize_t recvfrom(void* buf,
                           size_t len,
                           address& addr,
                           int timeout) = delete;

          ssize_t recvfrom(void* buf, size_t len, int timeout) = delete;

          // Send to.
          using net::sync::socket::sendto;
          bool sendto(const void* buf,
                      size_t len,
                      const address& addr,
                      int timeout) = delete;

          bool sendto(const void* buf,
                      size_t len,
                      const address::ipv4& addr,
                      int timeout) = delete;

          bool sendto(const void* buf,
                      size_t len,
                      const address::ipv6& addr,
                      int timeout) = delete;

          bool sendto(const void* buf,
                      size_t len,
                      const address::local& addr,
                      int timeout) = delete;

          bool sendto(const void* buf, size_t len, int timeout) = delete;
      };

      inline bool socket::connect(const address& addr, int timeout)
      {
        return ((net::socket::create(static_cast<socket::domain>(addr.family()),
                                     socket::type::stream)) &&
                (sync::socket::connect(addr, timeout)));
      }

      inline bool socket::connect(const address::ipv4& addr, int timeout)
      {
        return ((net::socket::create(static_cast<socket::domain>(addr.family()),
                                     socket::type::stream)) &&
                (sync::socket::connect(addr, timeout)));
      }

      inline bool socket::connect(const address::ipv6& addr, int timeout)
      {
        return ((net::socket::create(static_cast<socket::domain>(addr.family()),
                                     socket::type::stream)) &&
                (sync::socket::connect(addr, timeout)));
      }

      inline bool socket::connect(const address::local& addr, int timeout)
      {
        return ((net::socket::create(static_cast<socket::domain>(addr.family()),
                                     socket::type::stream)) &&
                (sync::socket::connect(addr, timeout)));
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

#endif // NET_SYNC_TCP_SOCKET_H
