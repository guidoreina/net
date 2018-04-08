#ifndef NET_ASYNC_SOCKET_H
#define NET_ASYNC_SOCKET_H

#include "net/socket.h"

namespace net {
  namespace async {
    class socket : public net::socket {
      public:
        // Connect.
        using net::socket::connect;
        bool connect(const address& addr, int timeout) = delete;
        bool connect(const address::ipv4& addr, int timeout) = delete;
        bool connect(const address::ipv6& addr, int timeout) = delete;
        bool connect(const address::local& addr, int timeout) = delete;

        // Accept.
        using net::socket::accept;
        bool accept(socket& sock, address& addr, int timeout) = delete;
        bool accept(socket& sock, int timeout) = delete;

        // Receive.
        using net::socket::recv;
        ssize_t recv(void* buf, size_t len, int timeout) = delete;

        // Send.
        using net::socket::send;
        bool send(const void* buf, size_t len, int timeout) = delete;

        // Read into multiple buffers.
        using net::socket::readv;
        ssize_t readv(const struct iovec* iov,
                      unsigned iovcnt,
                      int timeout) = delete;

        // Write from multiple buffers.
        using net::socket::writev;
        bool writev(const struct iovec* iov,
                    unsigned iovcnt,
                    int timeout) = delete;

        // Receive from.
        using net::socket::recvfrom;
        ssize_t recvfrom(void* buf,
                         size_t len,
                         address& addr,
                         int timeout) = delete;

        ssize_t recvfrom(void* buf, size_t len, int timeout) = delete;

        // Send to.
        using net::socket::sendto;
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

        // Receive message.
        using net::socket::recvmsg;
        ssize_t recvmsg(struct msghdr* msg, int timeout) = delete;

        // Send message.
        using net::socket::sendmsg;
        bool sendmsg(const struct msghdr* msg, int timeout) = delete;

#if defined(HAVE_RECVMMSG)
        // Receive multiple messages.
        using net::socket::recvmmsg;
        int recvmmsg(struct mmsghdr* msgvec,
                     unsigned vlen,
                     int timeout) = delete;
#endif // defined(HAVE_RECVMMSG)

#if defined(HAVE_SENDMMSG)
        // Send multiple messages.
        using net::socket::sendmmsg;
        bool sendmmsg(const struct mmsghdr* msgvec,
                      unsigned vlen,
                      int timeout) = delete;
#endif // defined(HAVE_SENDMMSG)

#if defined(HAVE_SENDFILE)
        // Send file.
        using net::socket::sendfile;
        bool sendfile(int in_fd,
                      off_t& offset,
                      size_t count,
                      int timeout) = delete;
#endif // defined(HAVE_SENDFILE)
    };
  }
}

#endif // NET_ASYNC_SOCKET_H
