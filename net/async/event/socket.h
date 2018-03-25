#ifndef NET_ASYNC_EVENT_SOCKET_H
#define NET_ASYNC_EVENT_SOCKET_H

#include <errno.h>
#include "net/async/socket.h"
#include "net/async/event/dispatcher.h"

#if !defined(USE_SOCKET_TEMPLATE)
  #define T socket
#endif

namespace net {
  namespace async {
    namespace event {
      class socket : private util::node {
        friend class dispatcher;

        public:
          // Constructor.
          socket(dispatcher* dispatcher);
          socket();

#if !defined(USE_SOCKET_TEMPLATE)
          // Destructor.
          virtual ~socket() = default;
#endif // !defined(USE_SOCKET_TEMPLATE)

          // Connect.
          bool connect(const net::socket::address& addr);
          bool connect(const net::socket::address::ipv4& addr);
          bool connect(const net::socket::address::ipv6& addr);
          bool connect(const net::socket::address::local& addr);
          bool connect(const net::socket::address& addr, unsigned timeout);
          bool connect(const net::socket::address::ipv4& addr,
                       unsigned timeout);

          bool connect(const net::socket::address::ipv6& addr,
                       unsigned timeout);

          bool connect(const net::socket::address::local& addr,
                       unsigned timeout);

          // Bind.
          bool bind(const net::socket::address& addr);
          bool bind(const net::socket::address::ipv4& addr);
          bool bind(const net::socket::address::ipv6& addr);
          bool bind(const net::socket::address::local& addr);
          bool bind(const net::socket::address& addr, unsigned timeout);
          bool bind(const net::socket::address::ipv4& addr, unsigned timeout);
          bool bind(const net::socket::address::ipv6& addr, unsigned timeout);
          bool bind(const net::socket::address::local& addr, unsigned timeout);

          // Listen.
          bool listen(const net::socket::address& addr);
          bool listen(const net::socket::address::ipv4& addr);
          bool listen(const net::socket::address::ipv6& addr);
          bool listen(const net::socket::address::local& addr);
          bool listen(const net::socket::address& addr, unsigned timeout);
          bool listen(const net::socket::address::ipv4& addr, unsigned timeout);
          bool listen(const net::socket::address::ipv6& addr, unsigned timeout);
          bool listen(const net::socket::address::local& addr,
                      unsigned timeout);

#if defined(USE_SOCKET_TEMPLATE)
          // Clear.
          void clear();

          // Timeout.
          // Return false if the socket should be closed; true otherwise.
          bool timeout();

          // Run.
          bool run();
#else
          // Clear.
          virtual void clear();

          // Timeout.
          // Return false if the socket should be closed; true otherwise.
          virtual bool timeout();

          // Run.
          virtual bool run() = 0;
#endif

          // Get handle.
          net::socket::handle_t handle() const;

        protected:
          int _M_timeout; // Milliseconds.

          // Get socket error.
          bool get_socket_error(int& error);

          // Accept.
#if defined(USE_SOCKET_TEMPLATE)
          template<typename T>
#endif
          bool accept(T& sock, net::socket::address& addr);

#if defined(USE_SOCKET_TEMPLATE)
          template<typename T>
#endif
          bool accept(T& sock);

#if defined(USE_SOCKET_TEMPLATE)
          template<typename T>
#endif
          bool accept(T& sock, net::socket::address& addr, unsigned timeout);

#if defined(USE_SOCKET_TEMPLATE)
          template<typename T>
#endif
          bool accept(T& sock, unsigned timeout);

          // Receive.
          ssize_t recv(void* buf, size_t len);

          // Send.
          ssize_t send(const void* buf, size_t len);

          // Read into multiple buffers.
          ssize_t readv(const struct iovec* iov, unsigned iovcnt);

          // Write from multiple buffers.
          ssize_t writev(const struct iovec* iov, unsigned iovcnt);

          // Receive from.
          ssize_t recvfrom(void* buf, size_t len, net::socket::address& addr);
          ssize_t recvfrom(void* buf, size_t len);

          // Send to.
          ssize_t sendto(const void* buf,
                         size_t len,
                         const net::socket::address& addr);

          ssize_t sendto(const void* buf,
                         size_t len,
                         const net::socket::address::ipv4& addr);

          ssize_t sendto(const void* buf,
                         size_t len,
                         const net::socket::address::ipv6& addr);

          ssize_t sendto(const void* buf,
                         size_t len,
                         const net::socket::address::local& addr);

          // Receive message.
          ssize_t recvmsg(struct msghdr* msg);

          // Send message.
          ssize_t sendmsg(const struct msghdr* msg);

#if defined(HAVE_RECVMMSG)
          // Receive multiple messages.
          int recvmmsg(struct mmsghdr* msgvec, unsigned vlen);
#endif // defined(HAVE_RECVMMSG)

#if defined(HAVE_SENDMMSG)
          // Send multiple messages.
          int sendmmsg(struct mmsghdr* msgvec, unsigned vlen);
#endif // defined(HAVE_SENDMMSG)

          // Readable?
          bool readable() const;

          // Writable?
          bool writable() const;

          // Error?
          bool error() const;

        private:
          async::socket _M_socket;

          bool _M_readable;
          bool _M_writable;
          bool _M_error;

          net::event::watch _M_event;

          uint64_t _M_timestamp;
          uint64_t _M_expire;

          dispatcher* _M_dispatcher;

          // Initialize.
          void init();

          // Connect.
          template<typename Address>
          bool connect_(const Address& addr);

          template<typename Address>
          bool connect_(const Address& addr, unsigned timeout);

          // Bind.
          template<typename Address>
          bool bind_(const Address& addr);

          template<typename Address>
          bool bind_(const Address& addr, unsigned timeout);

          // Listen.
          template<typename Address>
          bool listen_(const Address& addr);

          template<typename Address>
          bool listen_(const Address& addr, unsigned timeout);

          // Send to.
          template<typename Address>
          ssize_t sendto_(const void* buf, size_t len, const Address& addr);
      };

      inline socket::socket(dispatcher* dispatcher)
        : _M_dispatcher(dispatcher)
      {
        init();
      }

      inline socket::socket()
      {
        init();
      }

      inline bool socket::connect(const net::socket::address& addr)
      {
        return connect_(addr);
      }

      inline bool socket::connect(const net::socket::address::ipv4& addr)
      {
        return connect_(addr);
      }

      inline bool socket::connect(const net::socket::address::ipv6& addr)
      {
        return connect_(addr);
      }

      inline bool socket::connect(const net::socket::address::local& addr)
      {
        return connect_(addr);
      }

      inline bool socket::connect(const net::socket::address& addr,
                                  unsigned timeout)
      {
        return connect_(addr, timeout);
      }

      inline bool socket::connect(const net::socket::address::ipv4& addr,
                                  unsigned timeout)
      {
        return connect_(addr, timeout);
      }

      inline bool socket::connect(const net::socket::address::ipv6& addr,
                                  unsigned timeout)
      {
        return connect_(addr, timeout);
      }

      inline bool socket::connect(const net::socket::address::local& addr,
                                  unsigned timeout)
      {
        return connect_(addr, timeout);
      }

      inline bool socket::bind(const net::socket::address& addr)
      {
        return bind_(addr);
      }

      inline bool socket::bind(const net::socket::address::ipv4& addr)
      {
        return bind_(addr);
      }

      inline bool socket::bind(const net::socket::address::ipv6& addr)
      {
        return bind_(addr);
      }

      inline bool socket::bind(const net::socket::address::local& addr)
      {
        return bind_(addr);
      }

      inline bool socket::bind(const net::socket::address& addr,
                               unsigned timeout)
      {
        return bind_(addr, timeout);
      }

      inline bool socket::bind(const net::socket::address::ipv4& addr,
                               unsigned timeout)
      {
        return bind_(addr, timeout);
      }

      inline bool socket::bind(const net::socket::address::ipv6& addr,
                               unsigned timeout)
      {
        return bind_(addr, timeout);
      }

      inline bool socket::bind(const net::socket::address::local& addr,
                               unsigned timeout)
      {
        return bind_(addr, timeout);
      }

      inline bool socket::listen(const net::socket::address& addr)
      {
        return listen_(addr);
      }

      inline bool socket::listen(const net::socket::address::ipv4& addr)
      {
        return listen_(addr);
      }

      inline bool socket::listen(const net::socket::address::ipv6& addr)
      {
        return listen_(addr);
      }

      inline bool socket::listen(const net::socket::address::local& addr)
      {
        return listen_(addr);
      }

      inline bool socket::listen(const net::socket::address& addr,
                                 unsigned timeout)
      {
        return listen_(addr, timeout);
      }

      inline bool socket::listen(const net::socket::address::ipv4& addr,
                                 unsigned timeout)
      {
        return listen_(addr, timeout);
      }

      inline bool socket::listen(const net::socket::address::ipv6& addr,
                                 unsigned timeout)
      {
        return listen_(addr, timeout);
      }

      inline bool socket::listen(const net::socket::address::local& addr,
                                 unsigned timeout)
      {
        return listen_(addr, timeout);
      }

      inline void socket::clear()
      {
      }

      inline bool socket::timeout()
      {
        // Close socket.
        return false;
      }

#if defined(USE_SOCKET_TEMPLATE)
      inline bool socket::run()
      {
        return true;
      }
#endif // defined(USE_SOCKET_TEMPLATE)

      inline net::socket::handle_t socket::handle() const
      {
        return _M_socket.handle();
      }

      inline bool socket::get_socket_error(int& error)
      {
        return _M_socket.get_socket_error(error);
      }

#if defined(USE_SOCKET_TEMPLATE)
      template<typename T>
#endif
      inline bool socket::accept(T& sock, net::socket::address& addr)
      {
        if (_M_socket.accept(sock._M_socket, addr)) {
          if (_M_dispatcher->register_socket(&sock,
                                             net::event::watch::read_write)) {
            _M_timestamp = _M_dispatcher->time();

            sock._M_timestamp = _M_timestamp;
            sock._M_dispatcher = _M_dispatcher;

            return true;
          } else {
            sock._M_socket.close();
          }
        } else if (errno == EAGAIN) {
          _M_readable = false;
        } else {
          _M_error = true;
        }

        return false;
      }

#if defined(USE_SOCKET_TEMPLATE)
      template<typename T>
#endif
      inline bool socket::accept(T& sock)
      {
        if (_M_socket.accept(sock._M_socket)) {
          if (_M_dispatcher->register_socket(&sock,
                                             net::event::watch::read_write)) {
            _M_timestamp = _M_dispatcher->time();

            sock._M_timestamp = _M_timestamp;
            sock._M_dispatcher = _M_dispatcher;

            return true;
          } else {
            sock._M_socket.close();
          }
        } else if (errno == EAGAIN) {
          _M_readable = false;
        } else {
          _M_error = true;
        }

        return false;
      }

#if defined(USE_SOCKET_TEMPLATE)
      template<typename T>
#endif
      inline bool socket::accept(T& sock,
                                 net::socket::address& addr,
                                 unsigned timeout)
      {
        if (_M_socket.accept(sock._M_socket, addr)) {
          if (_M_dispatcher->register_socket(&sock,
                                             net::event::watch::read_write,
                                             timeout)) {
            _M_timestamp = _M_dispatcher->time();
            sock._M_dispatcher = _M_dispatcher;

            return true;
          } else {
            sock._M_socket.close();
          }
        } else if (errno == EAGAIN) {
          _M_readable = false;
        } else {
          _M_error = true;
        }

        return false;
      }

#if defined(USE_SOCKET_TEMPLATE)
      template<typename T>
#endif
      inline bool socket::accept(T& sock, unsigned timeout)
      {
        if (_M_socket.accept(sock._M_socket)) {
          if (_M_dispatcher->register_socket(&sock,
                                             net::event::watch::read_write,
                                             timeout)) {
            _M_timestamp = _M_dispatcher->time();
            sock._M_dispatcher = _M_dispatcher;

            return true;
          } else {
            sock._M_socket.close();
          }
        } else if (errno == EAGAIN) {
          _M_readable = false;
        } else {
          _M_error = true;
        }

        return false;
      }

      inline ssize_t socket::recv(void* buf, size_t len)
      {
        ssize_t ret;
        if ((ret = _M_socket.recv(buf, len)) == static_cast<ssize_t>(len)) {
          _M_timestamp = _M_dispatcher->time();
        } else if (ret >= 0) {
          _M_readable = false;
          _M_timestamp = _M_dispatcher->time();
        } else if (errno == EAGAIN) {
          _M_readable = false;
        } else {
          _M_error = true;
        }

        return ret;
      }

      inline ssize_t socket::send(const void* buf, size_t len)
      {
        ssize_t ret;
        if ((ret = _M_socket.send(buf, len)) == static_cast<ssize_t>(len)) {
          _M_timestamp = _M_dispatcher->time();
        } else if (ret >= 0) {
          _M_writable = false;
          _M_timestamp = _M_dispatcher->time();
        } else if (errno == EAGAIN) {
          _M_writable = false;
        } else {
          _M_error = true;
        }

        return ret;
      }

      inline ssize_t socket::recvfrom(void* buf,
                                      size_t len,
                                      net::socket::address& addr)
      {
        ssize_t ret;
        if ((ret = _M_socket.recvfrom(buf, len, addr)) != -1) {
          _M_timestamp = _M_dispatcher->time();
        } else if (errno == EAGAIN) {
          _M_readable = false;
        } else {
          _M_error = true;
        }

        return ret;
      }

      inline ssize_t socket::recvfrom(void* buf, size_t len)
      {
        ssize_t ret;
        if ((ret = _M_socket.recvfrom(buf, len)) != -1) {
          _M_timestamp = _M_dispatcher->time();
        } else if (errno == EAGAIN) {
          _M_readable = false;
        } else {
          _M_error = true;
        }

        return ret;
      }

      inline ssize_t socket::sendto(const void* buf,
                                    size_t len,
                                    const net::socket::address& addr)
      {
        return sendto_(buf, len, addr);
      }

      inline ssize_t socket::sendto(const void* buf,
                                    size_t len,
                                    const net::socket::address::ipv4& addr)
      {
        return sendto_(buf, len, addr);
      }

      inline ssize_t socket::sendto(const void* buf,
                                    size_t len,
                                    const net::socket::address::ipv6& addr)
      {
        return sendto_(buf, len, addr);
      }

      inline ssize_t socket::sendto(const void* buf,
                                    size_t len,
                                    const net::socket::address::local& addr)
      {
        return sendto_(buf, len, addr);
      }

      inline ssize_t socket::recvmsg(struct msghdr* msg)
      {
        ssize_t ret;
        if ((ret = _M_socket.recvmsg(msg)) != -1) {
          _M_timestamp = _M_dispatcher->time();
        } else if (errno == EAGAIN) {
          _M_readable = false;
        } else {
          _M_error = true;
        }

        return ret;
      }

#if defined(HAVE_RECVMMSG)
      inline int socket::recvmmsg(struct mmsghdr* msgvec, unsigned vlen)
      {
        int ret;
        if ((ret = _M_socket.recvmmsg(msgvec, vlen)) ==
            static_cast<int>(vlen)) {
          _M_timestamp = _M_dispatcher->time();
        } else if (ret >= 0) {
          _M_readable = false;
          _M_timestamp = _M_dispatcher->time();
        } else if (errno == EAGAIN) {
          _M_readable = false;
        } else {
          _M_error = true;
        }

        return ret;
      }
#endif // defined(HAVE_RECVMMSG)

#if defined(HAVE_SENDMMSG)
      inline int socket::sendmmsg(struct mmsghdr* msgvec, unsigned vlen)
      {
        int ret;
        if ((ret = _M_socket.sendmmsg(msgvec, vlen)) ==
            static_cast<int>(vlen)) {
          _M_timestamp = _M_dispatcher->time();
        } else if (ret >= 0) {
          _M_writable = false;
          _M_timestamp = _M_dispatcher->time();
        } else if (errno == EAGAIN) {
          _M_writable = false;
        } else {
          _M_error = true;
        }

        return ret;
      }
#endif // defined(HAVE_SENDMMSG)

      inline bool socket::readable() const
      {
        return _M_readable;
      }

      inline bool socket::writable() const
      {
        return _M_writable;
      }

      inline bool socket::error() const
      {
        return _M_error;
      }

      inline void socket::init()
      {
        _M_timeout = -1;
        _M_readable = false;
        _M_writable = false;
        _M_error = false;
        _M_timestamp = 0;
      }

      template<typename Address>
      bool socket::connect_(const Address& addr)
      {
        // Create socket.
        if (_M_socket.create(static_cast<net::socket::domain>(addr.family()),
                             net::socket::type::stream)) {
          // Connect.
          if (_M_socket.connect(addr)) {
            // Save current time.
            _M_timestamp = _M_dispatcher->time();

            // Register socket.
            if (_M_dispatcher->register_socket(this,
                                               net::event::watch::read_write)) {
              return true;
            }
          }

          _M_socket.close();
        }

        return false;
      }

      template<typename Address>
      bool socket::connect_(const Address& addr, unsigned timeout)
      {
        // Create socket.
        if (_M_socket.create(static_cast<net::socket::domain>(addr.family()),
                             net::socket::type::stream)) {
          // Connect.
          if (_M_socket.connect(addr)) {
            _M_event = net::event::watch::read_write;

            // Save current time.
            _M_timestamp = _M_dispatcher->time();

            _M_timeout = timeout;

            // Register socket.
            if (_M_dispatcher->register_socket(this)) {
              return true;
            }
          }

          _M_socket.close();
        }

        return false;
      }

      template<typename Address>
      bool socket::bind_(const Address& addr)
      {
        // Create socket.
        if (_M_socket.create(static_cast<net::socket::domain>(addr.family()),
                             net::socket::type::datagram)) {
          // Bind.
          if (_M_socket.bind(addr)) {
            // Save current time.
            _M_timestamp = _M_dispatcher->time();

            // Register socket.
            if (_M_dispatcher->register_socket(this,
                                               net::event::watch::read_write)) {
              return true;
            }
          }

          _M_socket.close();
        }

        return false;
      }

      template<typename Address>
      bool socket::bind_(const Address& addr, unsigned timeout)
      {
        // Create socket.
        if (_M_socket.create(static_cast<net::socket::domain>(addr.family()),
                             net::socket::type::datagram)) {
          // Bind.
          if (_M_socket.bind(addr)) {
            _M_event = net::event::watch::read_write;

            // Save current time.
            _M_timestamp = _M_dispatcher->time();

            _M_timeout = timeout;

            // Register socket.
            if (_M_dispatcher->register_socket(this)) {
              return true;
            }
          }

          _M_socket.close();
        }

        return false;
      }

      template<typename Address>
      bool socket::listen_(const Address& addr)
      {
        // Create socket.
        if (_M_socket.create(static_cast<net::socket::domain>(addr.family()),
                             net::socket::type::stream)) {
          // Bind and listen.
          if ((_M_socket.bind(addr)) && (_M_socket.listen())) {
            // Save current time.
            _M_timestamp = _M_dispatcher->time();

            // Register socket.
            if (_M_dispatcher->register_socket(this, net::event::watch::read)) {
              return true;
            }
          }

          _M_socket.close();
        }

        return false;
      }

      template<typename Address>
      bool socket::listen_(const Address& addr, unsigned timeout)
      {
        // Create socket.
        if (_M_socket.create(static_cast<net::socket::domain>(addr.family()),
                             net::socket::type::stream)) {
          // Bind and listen.
          if ((_M_socket.bind(addr)) && (_M_socket.listen())) {
            _M_event = net::event::watch::read;

            // Save current time.
            _M_timestamp = _M_dispatcher->time();

            _M_timeout = timeout;

            // Register socket.
            if (_M_dispatcher->register_socket(this)) {
              return true;
            }
          }

          _M_socket.close();
        }

        return false;
      }

      template<typename Address>
      inline ssize_t socket::sendto_(const void* buf,
                                     size_t len,
                                     const Address& addr)
      {
        ssize_t ret;
        if ((ret = _M_socket.sendto(buf, len, addr)) ==
            static_cast<ssize_t>(len)) {
          _M_timestamp = _M_dispatcher->time();
        } else if (ret >= 0) {
          _M_writable = false;
          _M_timestamp = _M_dispatcher->time();
        } else if (errno == EAGAIN) {
          _M_writable = false;
        } else {
          _M_error = true;
        }

        return ret;
      }
    }
  }
}

#if !defined(USE_SOCKET_TEMPLATE)
  #undef T
#endif

#endif // NET_ASYNC_EVENT_SOCKET_H
