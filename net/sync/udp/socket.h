#ifndef NET_SYNC_UDP_SOCKET_H
#define NET_SYNC_UDP_SOCKET_H

#include "net/sync/socket.h"

namespace net {
  namespace sync {
    namespace udp {
      class socket : public net::sync::socket {
        public:
          // Create socket.
          using net::sync::socket::create;
          bool create(domain d, type t) = delete;
          bool create(domain d);

          // Connect.
          using net::sync::socket::connect;
          bool connect(const address& addr, int timeout) = delete;
          bool connect(const address::ipv4& addr, int timeout) = delete;
          bool connect(const address::ipv6& addr, int timeout) = delete;
          bool connect(const address::local& addr, int timeout) = delete;

          // Listen.
          using net::sync::socket::listen;
          bool listen() = delete;

          // Accept.
          using net::sync::socket::accept;
          bool accept(socket& sock, address& addr, int timeout) = delete;
          bool accept(socket& sock, int timeout) = delete;

          // Receive.
          using net::sync::socket::recv;
          ssize_t recv(void* buf, size_t len, int timeout) = delete;

          // Send.
          using net::sync::socket::send;
          bool send(const void* buf, size_t len, int timeout) = delete;

          // Read into multiple buffers.
          using net::sync::socket::readv;
          ssize_t readv(const struct iovec* iov,
                        unsigned iovcnt,
                        int timeout) = delete;

          // Write from multiple buffers.
          using net::sync::socket::writev;
          bool writev(const struct iovec* iov,
                      unsigned iovcnt,
                      int timeout) = delete;
      };

      inline bool socket::create(domain d)
      {
        return net::socket::create(d, socket::type::datagram);
      }
    }
  }
}

#endif // NET_SYNC_UDP_SOCKET_H
