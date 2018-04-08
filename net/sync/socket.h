#ifndef NET_SYNC_SOCKET_H
#define NET_SYNC_SOCKET_H

#include "net/socket.h"

namespace net {
  namespace sync {
    class socket : public net::socket {
      public:
        // Connect.
        using net::socket::connect;
        bool connect(const address& addr) = delete;
        bool connect(const address::ipv4& addr) = delete;
        bool connect(const address::ipv6& addr) = delete;
        bool connect(const address::local& addr) = delete;

        // Accept.
        using net::socket::accept;
        bool accept(socket& sock, address& addr) = delete;
        bool accept(socket& sock) = delete;

        // Receive.
        using net::socket::recv;
        ssize_t recv(void* buf, size_t len) = delete;

        // Send.
        using net::socket::send;
        ssize_t send(const void* buf, size_t len) = delete;

        // Read into multiple buffers.
        using net::socket::readv;
        ssize_t readv(const struct iovec* iov, unsigned iovcnt) = delete;

        // Write from multiple buffers.
        using net::socket::writev;
        ssize_t writev(const struct iovec* iov, unsigned iovcnt) = delete;

        // Receive from.
        using net::socket::recvfrom;
        ssize_t recvfrom(void* buf, size_t len, address& addr) = delete;
        ssize_t recvfrom(void* buf, size_t len) = delete;

        // Send to.
        using net::socket::sendto;
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

        // Receive message.
        using net::socket::recvmsg;
        ssize_t recvmsg(struct msghdr* msg) = delete;

        // Send message.
        using net::socket::sendmsg;
        ssize_t sendmsg(const struct msghdr* msg) = delete;

#if defined(HAVE_RECVMMSG)
        // Receive multiple messages.
        using net::socket::recvmmsg;
        int recvmmsg(struct mmsghdr* msgvec, unsigned vlen) = delete;
#endif // defined(HAVE_RECVMMSG)

#if defined(HAVE_SENDMMSG)
        // Send multiple messages.
        using net::socket::sendmmsg;
        int sendmmsg(struct mmsghdr* msgvec, unsigned vlen) = delete;
#endif // defined(HAVE_SENDMMSG)

#if defined(HAVE_SENDFILE)
        // Send file.
        using net::socket::sendfile;
        ssize_t sendfile(int in_fd, off_t& offset, size_t count) = delete;
#endif // defined(HAVE_SENDFILE)
    };
  }
}

#endif // NET_SYNC_SOCKET_H
