#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <poll.h>
#include <netinet/tcp.h>
#include <errno.h>
#include <limits.h>

#if defined(HAVE_SENDFILE) && defined(__linux__)
  #include <sys/sendfile.h>
#endif

#include "net/internal/socket/socket.h"

#if !defined(POLLRDHUP)
  #define POLLRDHUP 0
#endif

namespace net {
  namespace internal {
    namespace socket {
      static bool sendmsg(handle_t sock,
                          struct msghdr* msg,
                          int flags,
                          int timeout)
      {
        size_t iovcnt;
        if ((iovcnt = msg->msg_iovlen) <= IOV_MAX) {
          struct iovec vec[IOV_MAX];
          size_t total = 0;

          for (size_t i = 0; i < iovcnt; i++) {
            vec[i] = msg->msg_iov[i];

            total += vec[i].iov_len;
          }

          msg->msg_iov = vec;

          size_t sent = 0;

          do {
            ssize_t ret;
            if ((ret = socket::sendmsg(sock, msg, flags)) >= 0) {
              if ((sent += ret) == total) {
                return true;
              }

              if (wait_writable(sock, timeout)) {
                while (static_cast<size_t>(ret) >= msg->msg_iov->iov_len) {
                  ret -= msg->msg_iov->iov_len;

                  msg->msg_iov++;
                  msg->msg_iovlen--;
                }

                if (ret > 0) {
                  msg->msg_iov->iov_base = reinterpret_cast<uint8_t*>(
                                             msg->msg_iov->iov_base
                                           ) + ret;

                  msg->msg_iov->iov_len -= ret;
                }
              } else {
                return false;
              }
            } else {
              if ((errno != EAGAIN) || (!wait_writable(sock, timeout))) {
                return false;
              }
            }
          } while (true);
        } else {
          errno = EINVAL;
          return false;
        }
      }

      handle_t create(int domain, int type, int protocol)
      {
        // Always create non-blocking sockets.
        return ::socket(domain, type | SOCK_NONBLOCK, protocol);
      }

      bool close(handle_t sock)
      {
        return (::close(sock) == 0);
      }

      bool shutdown(handle_t sock, int how)
      {
        return (::shutdown(sock, how) == 0);
      }

      bool connect(handle_t sock,
                   const struct sockaddr* addr,
                   socklen_t addrlen)
      {
        int ret;
        while (((ret = ::connect(sock, addr, addrlen)) < 0) &&
               (errno == EINTR));

        return ((ret == 0) || (errno == EINPROGRESS));
      }

      bool connect(handle_t sock,
                   const struct sockaddr* addr,
                   socklen_t addrlen,
                   int timeout)
      {
        int ret;
        while (((ret = ::connect(sock, addr, addrlen)) < 0) &&
               (errno == EINTR));

        if ((ret == 0) ||
            ((errno == EINPROGRESS) && (wait_writable(sock, timeout)))) {
          int error;
          return ((get_socket_error(sock, error)) && (error == 0));
        } else {
          return false;
        }
      }

      handle_t connect(const struct sockaddr* addr, socklen_t addrlen)
      {
        handle_t sock;
        if ((sock = create(addr->sa_family, SOCK_STREAM)) != invalid_handle) {
          if (socket::connect(sock, addr, addrlen)) {
            return sock;
          }

          socket::close(sock);
        }

        return invalid_handle;
      }

      handle_t connect(const struct sockaddr* addr,
                       socklen_t addrlen,
                       int timeout)
      {
        handle_t sock;
        if ((sock = create(addr->sa_family, SOCK_STREAM)) != invalid_handle) {
          if (socket::connect(sock, addr, addrlen, timeout)) {
            return sock;
          }

          socket::close(sock);
        }

        return invalid_handle;
      }

      bool get_socket_error(handle_t sock, int& error)
      {
        socklen_t optlen = sizeof(int);
        return (::getsockopt(sock, SOL_SOCKET, SO_ERROR, &error, &optlen) == 0);
      }

      bool get_recvbuf_size(handle_t sock, int& size)
      {
        socklen_t optlen = sizeof(int);
        return (::getsockopt(sock, SOL_SOCKET, SO_RCVBUF, &size, &optlen) == 0);
      }

      bool set_recvbuf_size(handle_t sock, int size)
      {
        return (::setsockopt(sock,
                             SOL_SOCKET,
                             SO_RCVBUF,
                             &size,
                             sizeof(int)) == 0);
      }

      bool get_sendbuf_size(handle_t sock, int& size)
      {
        socklen_t optlen = sizeof(int);
        return (::getsockopt(sock, SOL_SOCKET, SO_SNDBUF, &size, &optlen) == 0);
      }

      bool set_sendbuf_size(handle_t sock, int size)
      {
        return (::setsockopt(sock,
                             SOL_SOCKET,
                             SO_SNDBUF,
                             &size,
                             sizeof(int)) == 0);
      }

      bool get_keep_alive(handle_t sock, bool& on)
      {
        int optval;
        socklen_t optlen = sizeof(int);

        if (::getsockopt(sock,
                         SOL_SOCKET,
                         SO_KEEPALIVE,
                         &optval,
                         &optlen) == 0) {
          on = (optval != 0);

          return true;
        }

        return false;
      }

      bool set_keep_alive(handle_t sock, bool on)
      {
        int optval = on;
        return (::setsockopt(sock,
                             SOL_SOCKET,
                             SO_KEEPALIVE,
                             &optval,
                             sizeof(int)) == 0);
      }

      bool get_tcp_no_delay(handle_t sock, bool& on)
      {
#if defined(TCP_NODELAY)
        int optval;
        socklen_t optlen = sizeof(int);

        if (::getsockopt(sock,
                         IPPROTO_TCP,
                         TCP_NODELAY,
                         &optval,
                         &optlen) == 0) {
          on = (optval != 0);

          return true;
        }
#endif // defined(TCP_NODELAY)

        return false;
      }

      bool set_tcp_no_delay(handle_t sock, bool on)
      {
#if defined(TCP_NODELAY)
        int optval = on;
        return (::setsockopt(sock,
                             IPPROTO_TCP,
                             TCP_NODELAY,
                             &optval,
                             sizeof(int)) == 0);
#else
        return false;
#endif
      }

      bool cork(handle_t sock)
      {
#if defined(TCP_CORK)
        int optval = 1;
        return (::setsockopt(sock,
                             IPPROTO_TCP,
                             TCP_CORK,
                             &optval,
                             sizeof(int)) == 0);
#elif defined(TCP_NOPUSH)
        int optval = 1;
        return (::setsockopt(sock,
                             IPPROTO_TCP,
                             TCP_NOPUSH,
                             &optval,
                             sizeof(int)) == 0);
#else
        return false;
#endif
      }

      bool uncork(handle_t sock)
      {
#if defined(TCP_CORK)
        int optval = 0;
        return (::setsockopt(sock,
                             IPPROTO_TCP,
                             TCP_CORK,
                             &optval,
                             sizeof(int)) == 0);
#elif defined(TCP_NOPUSH)
        int optval = 0;
        return (::setsockopt(sock,
                             IPPROTO_TCP,
                             TCP_NOPUSH,
                             &optval,
                             sizeof(int)) == 0);
#else
        return false;
#endif
      }

      bool bind(handle_t sock, const struct sockaddr* addr, socklen_t addrlen)
      {
        // Reuse address and port.
        int optval = 1;
        return ((::setsockopt(sock,
                              SOL_SOCKET,
                              SO_REUSEADDR,
                              &optval,
                              sizeof(int)) == 0) &&
                (::setsockopt(sock,
                              SOL_SOCKET,
                              SO_REUSEPORT,
                              &optval,
                              sizeof(int)) == 0) &&
                (::bind(sock, addr, addrlen) == 0));
      }

      bool listen(handle_t sock)
      {
        return (::listen(sock, SOMAXCONN) == 0);
      }

      handle_t listen(const struct sockaddr* addr, socklen_t addrlen)
      {
        handle_t sock;
        if ((sock = create(addr->sa_family, SOCK_STREAM)) != invalid_handle) {
          if ((socket::bind(sock, addr, addrlen)) && (socket::listen(sock))) {
            return sock;
          }

          socket::close(sock);
        }

        return invalid_handle;
      }

      handle_t accept(handle_t sock, struct sockaddr* addr, socklen_t* addrlen)
      {
        handle_t s;

#if defined(HAVE_ACCEPT4)
        while (((s = ::accept4(sock, addr, addrlen, SOCK_NONBLOCK)) < 0) &&
               (errno == EINTR));
#elif defined(HAVE_PACCEPT)
        while (((s = ::paccept(sock,
                               addr,
                               addrlen,
                               nullptr,
                               SOCK_NONBLOCK)) < 0) &&
               (errno == EINTR));
#else
        while (((s = ::accept(sock, addr, addrlen)) < 0) && (errno == EINTR));

        if (s != invalid_handle) {
          // Make socket non-blocking.

          int flags;
          if (((flags = fcntl(s, F_GETFL)) < 0) ||
              (fcntl(s, F_SETFL, flags | O_NONBLOCK) < 0)) {
            socket::close(s);
            return invalid_handle;
          }
        }
#endif

        return s;
      }

      handle_t accept(handle_t sock,
                      struct sockaddr* addr,
                      socklen_t* addrlen,
                      int timeout)
      {
        handle_t s;
        if ((s = socket::accept(sock, addr, addrlen)) != invalid_handle) {
          return s;
        } else if ((errno == EAGAIN) && (wait_readable(sock, timeout))) {
          return socket::accept(sock, addr, addrlen);
        } else {
          return invalid_handle;
        }
      }

      ssize_t recv(handle_t sock, void* buf, size_t len, int flags)
      {
        ssize_t ret;
        while (((ret = ::recv(sock, buf, len, flags)) < 0) && (errno == EINTR));
        return ret;
      }

      ssize_t recv(handle_t sock, void* buf, size_t len, int flags, int timeout)
      {
        ssize_t ret;
        if ((ret = socket::recv(sock, buf, len, flags)) != -1) {
          return ret;
        } else if ((errno == EAGAIN) && (wait_readable(sock, timeout))) {
          return socket::recv(sock, buf, len, flags);
        } else {
          return -1;
        }
      }

      ssize_t send(handle_t sock, const void* buf, size_t len, int flags)
      {
        ssize_t ret;
        while (((ret = ::send(sock,
                              buf,
                              len,
                              flags | MSG_NOSIGNAL)) < 0) &&
               (errno == EINTR));

        return ret;
      }

      bool send(handle_t sock,
                const void* buf,
                size_t len,
                int flags,
                int timeout)
      {
        const uint8_t* b = reinterpret_cast<const uint8_t*>(buf);

        do {
          ssize_t ret;
          if ((ret = socket::send(sock, b, len, flags)) >= 0) {
            if ((len -= ret) == 0) {
              return true;
            }

            if (wait_writable(sock, timeout)) {
              b += ret;
            } else {
              return false;
            }
          } else {
            if ((errno != EAGAIN) || (!wait_writable(sock, timeout))) {
              return false;
            }
          }
        } while (true);
      }

      ssize_t readv(handle_t sock, const struct iovec* iov, unsigned iovcnt)
      {
        ssize_t ret;
        while (((ret = ::readv(sock, iov, iovcnt)) < 0) && (errno == EINTR));
        return ret;
      }

      ssize_t readv(handle_t sock,
                    const struct iovec* iov,
                    unsigned iovcnt,
                    int timeout)
      {
        ssize_t ret;
        if ((ret = socket::readv(sock, iov, iovcnt)) != -1) {
          return ret;
        } else if ((errno == EAGAIN) && (wait_readable(sock, timeout))) {
          return socket::readv(sock, iov, iovcnt);
        } else {
          return -1;
        }
      }

      ssize_t writev(handle_t sock, const struct iovec* iov, unsigned iovcnt)
      {
        struct msghdr msg;
        msg.msg_name = nullptr;
        msg.msg_namelen = 0;
        msg.msg_iov = const_cast<struct iovec*>(iov);
        msg.msg_iovlen = iovcnt;
        msg.msg_control = nullptr;
        msg.msg_controllen = 0;
        msg.msg_flags = 0;

        return socket::sendmsg(sock, &msg);
      }

      bool writev(handle_t sock,
                  const struct iovec* iov,
                  unsigned iovcnt,
                  int timeout)
      {
        struct msghdr msg;
        msg.msg_name = nullptr;
        msg.msg_namelen = 0;
        msg.msg_iov = const_cast<struct iovec*>(iov);
        msg.msg_iovlen = iovcnt;
        msg.msg_control = nullptr;
        msg.msg_controllen = 0;
        msg.msg_flags = 0;

        return socket::sendmsg(sock, &msg, 0, timeout);
      }

      ssize_t recvfrom(handle_t sock,
                       void* buf,
                       size_t len,
                       struct sockaddr* addr,
                       socklen_t* addrlen,
                       int flags)
      {
        ssize_t ret;
        while (((ret = ::recvfrom(sock, buf, len, flags, addr, addrlen)) < 0) &&
               (errno == EINTR));

        return ret;
      }

      ssize_t recvfrom(handle_t sock,
                       void* buf,
                       size_t len,
                       struct sockaddr* addr,
                       socklen_t* addrlen,
                       int flags,
                       int timeout)
      {
        ssize_t ret;
        if ((ret = socket::recvfrom(sock,
                                    buf,
                                    len,
                                    addr,
                                    addrlen,
                                    flags)) != -1) {
          return ret;
        } else if ((errno == EAGAIN) && (wait_readable(sock, timeout))) {
          return socket::recvfrom(sock, buf, len, addr, addrlen, flags);
        } else {
          return -1;
        }
      }

      ssize_t sendto(handle_t sock,
                     const void* buf,
                     size_t len,
                     const struct sockaddr* addr,
                     socklen_t addrlen,
                     int flags)
      {
        ssize_t ret;
        while (((ret = ::sendto(sock,
                                buf,
                                len,
                                flags | MSG_NOSIGNAL,
                                addr,
                                addrlen)) < 0) &&
               (errno == EINTR));

        return ret;
      }

      bool sendto(handle_t sock,
                  const void* buf,
                  size_t len,
                  const struct sockaddr* addr,
                  socklen_t addrlen,
                  int flags,
                  int timeout)
      {
        const uint8_t* b = reinterpret_cast<const uint8_t*>(buf);

        do {
          ssize_t ret;
          if ((ret = socket::sendto(sock,
                                    b,
                                    len,
                                    addr,
                                    addrlen,
                                    flags)) >= 0) {
            if ((len -= ret) == 0) {
              return true;
            }

            if (wait_writable(sock, timeout)) {
              b += ret;
            } else {
              return false;
            }
          } else {
            if ((errno != EAGAIN) || (!wait_writable(sock, timeout))) {
              return false;
            }
          }
        } while (true);
      }

      ssize_t recvmsg(handle_t sock, struct msghdr* msg, int flags)
      {
        ssize_t ret;
        while (((ret = ::recvmsg(sock, msg, flags)) < 0) &&
               (errno == EINTR));

        return ret;
      }

      ssize_t recvmsg(handle_t sock, struct msghdr* msg, int flags, int timeout)
      {
        ssize_t ret;
        if ((ret = socket::recvmsg(sock, msg, flags)) != -1) {
          return ret;
        } else if ((errno == EAGAIN) && (wait_readable(sock, timeout))) {
          return socket::recvmsg(sock, msg, flags);
        } else {
          return -1;
        }
      }

      ssize_t sendmsg(handle_t sock, const struct msghdr* msg, int flags)
      {
        ssize_t ret;
        while (((ret = ::sendmsg(sock, msg, flags | MSG_NOSIGNAL)) < 0) &&
               (errno == EINTR));

        return ret;
      }

      bool sendmsg(handle_t sock,
                   const struct msghdr* msg,
                   int flags,
                   int timeout)
      {
        struct msghdr m = *msg;
        return socket::sendmsg(sock, &m, flags, timeout);
      }

#if defined(HAVE_RECVMMSG)
      int recvmmsg(handle_t sock,
                   struct mmsghdr* msgvec,
                   unsigned vlen,
                   unsigned flags)
      {
        // For SOCK_STREAM sockets: recvmmsg() returns vlen and all the
        // mmsghdr::msg_len == 0 when the peer has closed the connection.
        int ret;
        while (((ret = ::recvmmsg(sock, msgvec, vlen, flags, nullptr)) < 0) &&
               (errno == EINTR));

        return ret;
      }

      int recvmmsg(handle_t sock,
                   struct mmsghdr* msgvec,
                   unsigned vlen,
                   unsigned flags,
                   int timeout)
      {
        int ret;
        if ((ret = socket::recvmmsg(sock, msgvec, vlen, flags)) != -1) {
          return ret;
        } else if ((errno == EAGAIN) && (wait_readable(sock, timeout))) {
          return socket::recvmmsg(sock, msgvec, vlen, flags);
        } else {
          return -1;
        }
      }
#endif // defined(HAVE_RECVMMSG)

#if defined(HAVE_SENDMMSG)
      int sendmmsg(handle_t sock,
                   struct mmsghdr* msgvec,
                   unsigned vlen,
                   unsigned flags)
      {
        int ret;
        while (((ret = ::sendmmsg(sock,
                                  msgvec,
                                  vlen,
                                  flags | MSG_NOSIGNAL)) < 0) &&
               (errno == EINTR));

        return ret;
      }

      bool sendmmsg(handle_t sock,
                    const struct mmsghdr* msgvec,
                    unsigned vlen,
                    unsigned flags,
                    int timeout)
      {
        static const size_t uio_maxiov = 16;

        // Check whether some message has too many elements in msg_iov.
        for (unsigned i = 0; i < vlen; i++) {
          if (msgvec[i].msg_hdr.msg_iovlen > IOV_MAX) {
            errno = EINVAL;
            return false;
          }
        }

        while (vlen > 0) {
          struct mmsghdr msgs[uio_maxiov];
          struct iovec vec[uio_maxiov][IOV_MAX];
          size_t lengths[uio_maxiov];

          unsigned nmsgs = (vlen < uio_maxiov) ? vlen : uio_maxiov;

          for (unsigned i = 0; i < nmsgs; i++) {
            const struct msghdr* src = &msgvec[i].msg_hdr;
            size_t msg_iovlen = src->msg_iovlen;

            struct msghdr* dest = &msgs[i].msg_hdr;

            dest->msg_name = src->msg_name;
            dest->msg_namelen = src->msg_namelen;
            dest->msg_iov = vec[i];
            dest->msg_iovlen = msg_iovlen;
            dest->msg_control = src->msg_control;
            dest->msg_controllen = src->msg_controllen;
            dest->msg_flags = src->msg_flags;

            size_t len = 0;
            for (size_t j = 0; j < msg_iovlen; j++) {
              vec[i][j] = src->msg_iov[j];

              len += vec[i][j].iov_len;
            }

            lengths[i] = len;
          }

          msgvec += nmsgs;
          vlen -= nmsgs;

          struct mmsghdr* m = msgs;
          size_t* l = lengths;

          do {
            int ret;
            if ((ret = socket::sendmmsg(sock,
                                        m,
                                        nmsgs,
                                        flags)) >= 0) {
              // Skip messages which have been completely sent.
              int i;

#if !defined(__FreeBSD__)
              for (i = 0; (i < ret) && (m[i].msg_len == l[i]); i++);
#else
              for (i = 0;
                   (i < ret) && (m[i].msg_len == static_cast<ssize_t>(l[i]));
                   i++);
#endif

              // If all the messages have been sent...
              if ((static_cast<unsigned>(ret) == nmsgs) && (i == ret)) {
                break;
              }

              nmsgs -= i;

              m += i;
              l += i;
              ret -= i;

              // Check rest of the messages.
              for (i = 0; i < ret; i++) {
                struct mmsghdr* msg = m + i;

                // If not all the data has been sent...
                if ((l[i] -= msg->msg_len) > 0) {
                  struct msghdr* hdr = &msg->msg_hdr;

#if !defined(__FreeBSD__)
                  while (msg->msg_len >= hdr->msg_iov->iov_len) {
#else
                  while (msg->msg_len >=
                         static_cast<ssize_t>(hdr->msg_iov->iov_len)) {
#endif
                    msg->msg_len -= hdr->msg_iov->iov_len;

                    hdr->msg_iov++;
                    hdr->msg_iovlen--;
                  }

                  if (msg->msg_len > 0) {
                    hdr->msg_iov->iov_base = reinterpret_cast<uint8_t*>(
                                               hdr->msg_iov->iov_base
                                             ) + msg->msg_len;

                    hdr->msg_iov->iov_len -= msg->msg_len;
                  }
                } else {
                  msg->msg_hdr.msg_iovlen = 0;
                }
              }

              if (!wait_writable(sock, timeout)) {
                return false;
              }
            } else {
              if ((errno != EAGAIN) || (!wait_writable(sock, timeout))) {
                return false;
              }
            }
          } while (true);
        }

        return true;
      }
#endif // defined(HAVE_SENDMMSG)

#if defined(HAVE_SENDFILE)
      ssize_t sendfile(handle_t sock,
                       int in_fd,
                       off_t* offset,
                       size_t count)
      {
#if defined(__linux__)
        ssize_t ret;
        while (((ret = ::sendfile(sock,
                                  in_fd,
                                  offset,
                                  count)) < 0) &&
               (errno == EINTR));

        return ret;
#elif defined(__FreeBSD__) || defined(__DragonFly__)
        int ret;
        off_t sbytes;
        size_t sent = 0;

        while (((ret = ::sendfile(in_fd,
                                  sock,
                                  *offset,
                                  count - sent,
                                  nullptr,
                                  &sbytes,
                                  0)) < 0) &&
               (errno == EINTR)) {
          *offset += sbytes;
          sent += sbytes;
        }

        *offset += sbytes;
        sent += sbytes;

        return (sent > 0) ? sent : ret;
#endif
      }

      bool sendfile(handle_t sock,
                    int in_fd,
                    off_t* offset,
                    size_t count,
                    int timeout)
      {
        do {
          ssize_t ret;
          if ((ret = socket::sendfile(sock, in_fd, offset, count)) >= 0) {
            if ((count -= ret) == 0) {
              return true;
            }

            if (!wait_writable(sock, timeout)) {
              return false;
            }
          } else {
            if ((errno != EAGAIN) || (!wait_writable(sock, timeout))) {
              return false;
            }
          }
        } while (true);
      }
#endif // defined(HAVE_SENDFILE)

      bool wait_readable(handle_t sock, int timeout)
      {
        struct pollfd fd;
        fd.fd = sock;
        fd.events = POLLIN | POLLRDHUP;
        fd.revents = 0;

        switch (::poll(&fd, 1, timeout)) {
          case 1:
            return true;
          case 0: // Timeout.
            errno = ETIMEDOUT;

            // Fall through.
          default:
            return false;
        }
      }

      bool wait_writable(handle_t sock, int timeout)
      {
        struct pollfd fd;
        fd.fd = sock;
        fd.events = POLLOUT;
        fd.revents = 0;

        switch (::poll(&fd, 1, timeout)) {
          case 1:
            return true;
          case 0: // Timeout.
            errno = ETIMEDOUT;

            // Fall through.
          default:
            return false;
        }
      }
    }
  }
}
