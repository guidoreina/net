#ifndef NET_ASYNC_UDP_SOCKET_H
#define NET_ASYNC_UDP_SOCKET_H

#include "net/async/socket.h"

namespace net {
  namespace async {
    namespace udp {
      class socket : public net::async::socket {
        public:
          // Create socket.
          using net::async::socket::create;
          bool create(domain d, type t) = delete;
          bool create(domain d);

          // Connect.
          using net::async::socket::connect;
          bool connect(const address& addr) = delete;
          bool connect(const address::ipv4& addr) = delete;
          bool connect(const address::ipv6& addr) = delete;
          bool connect(const address::local& addr) = delete;

          // Listen.
          using net::async::socket::listen;
          bool listen() = delete;

          // Accept.
          using net::async::socket::accept;
          bool accept(socket& sock, address& addr) = delete;
          bool accept(socket& sock) = delete;

          // Receive.
          using net::async::socket::recv;
          ssize_t recv(void* buf, size_t len) = delete;

          // Send.
          using net::async::socket::send;
          ssize_t send(const void* buf, size_t len) = delete;

          // Read into multiple buffers.
          using net::async::socket::readv;
          ssize_t readv(const struct iovec* iov, unsigned iovcnt) = delete;

          // Write from multiple buffers.
          using net::async::socket::writev;
          ssize_t writev(const struct iovec* iov, unsigned iovcnt) = delete;
      };

      inline bool socket::create(domain d)
      {
        return net::socket::create(d, socket::type::datagram);
      }
    }
  }
}

#endif // NET_ASYNC_UDP_SOCKET_H
