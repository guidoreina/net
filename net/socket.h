#ifndef NET_SOCKET_H
#define NET_SOCKET_H

#include <stddef.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "net/internal/socket/socket.h"
#include "net/internal/socket/address/address.h"

namespace net {
  class socket {
    public:
      class address {
        friend class socket;

        public:
          class ipv4 {
            public:
              // Constructor.
              ipv4();
              ipv4(const ipv4& addr);
              ipv4(const struct sockaddr_in& addr);

              // Get family.
              static sa_family_t family();

              // Get port.
              in_port_t port() const;

              // Set port.
              void port(in_port_t p);

              // Get size.
              static socklen_t size();

              // Build.
              bool build(const char* address, in_port_t port);
              bool build(const char* address);

              // Comparison operator.
              bool operator==(const ipv4& addr) const;
              bool operator==(const struct sockaddr_in& addr) const;

              // Assignment operator.
              ipv4& operator=(const ipv4& addr);
              ipv4& operator=(const struct sockaddr_in& addr);

              // To string.
              const char* to_string(char* s, size_t n) const;

              // Cast operators.
              operator const struct sockaddr*() const;
              operator struct sockaddr*();

              operator const struct sockaddr&() const;
              operator struct sockaddr&();

            private:
              struct sockaddr_in _M_addr;
          };

          class ipv6 {
            public:
              // Constructor.
              ipv6();
              ipv6(const ipv6& addr);
              ipv6(const struct sockaddr_in6& addr);

              // Get family.
              static sa_family_t family();

              // Get port.
              in_port_t port() const;

              // Set port.
              void port(in_port_t p);

              // Get size.
              static socklen_t size();

              // Build.
              bool build(const char* address, in_port_t port);
              bool build(const char* address);

              // Comparison operator.
              bool operator==(const ipv6& addr) const;
              bool operator==(const struct sockaddr_in6& addr) const;

              // Assignment operator.
              ipv6& operator=(const ipv6& addr);
              ipv6& operator=(const struct sockaddr_in6& addr);

              // To string.
              const char* to_string(char* s, size_t n) const;

              // Cast operators.
              operator const struct sockaddr*() const;
              operator struct sockaddr*();

              operator const struct sockaddr&() const;
              operator struct sockaddr&();

            private:
              struct sockaddr_in6 _M_addr;
          };

          class local {
            public:
              // Constructor.
              local();
              local(const local& addr);
              local(const struct sockaddr_un& addr);

              // Get family.
              static sa_family_t family();

              // Get size.
              socklen_t size() const;

              // Build.
              bool build(const char* address);
              bool build(const void* address, size_t len);

              // Comparison operator.
              bool operator==(const local& addr) const;
              bool operator==(const struct sockaddr_un& addr) const;

              // Assignment operator.
              local& operator=(const local& addr);
              local& operator=(const struct sockaddr_un& addr);

              // To string.
              const char* to_string(char* s, size_t n) const;

              // Cast operators.
              operator const struct sockaddr*() const;
              operator struct sockaddr*();

              operator const struct sockaddr&() const;
              operator struct sockaddr&();

            private:
              struct sockaddr_un _M_addr;
              socklen_t _M_addrlen;
          };

          // Constructor.
          address();
          address(const address& addr);
          address(const struct sockaddr& addr);

          // Get family.
          sa_family_t family() const;

          // Get size.
          socklen_t size() const;

          // Build.
          bool build(const char* address, in_port_t port);
          bool build(const char* address);

          // Comparison operator.
          bool operator==(const address& addr) const;
          bool operator==(const struct sockaddr& addr) const;

          // Assignment operator.
          address& operator=(const address& addr);
          address& operator=(const struct sockaddr& addr);

          // To string.
          const char* to_string(char* s, size_t n) const;

          // Cast operators.
          operator const struct sockaddr*() const;
          operator struct sockaddr*();

          operator const struct sockaddr&() const;
          operator struct sockaddr&();

        private:
          struct sockaddr_storage _M_addr;
          socklen_t _M_addrlen;
      };

      static const int default_timeout = 30 * 1000; // Milliseconds.

      typedef internal::socket::handle_t handle_t;
      static const handle_t invalid_handle = internal::socket::invalid_handle;

      // Constructor.
      socket();
      socket(handle_t h);

      // Destructor.
      ~socket();

      // Create socket.
      enum class domain {
        local  = AF_UNIX,
        ipv4   = AF_INET,
        ipv6   = AF_INET6
#if defined(AF_PACKET)
        , packet = AF_PACKET
#endif
      };

      enum class type {
        stream    = SOCK_STREAM,
        datagram  = SOCK_DGRAM,
        seqpacket = SOCK_SEQPACKET,
        raw       = SOCK_RAW
#if defined(SOCK_PACKET)
        , packet    = SOCK_PACKET
#endif
      };

      bool create(domain d, type t);

      // Close socket.
      void close();

      // Shutdown socket.
      enum class shutdown_how {
        read       = SHUT_RD,
        write      = SHUT_WR,
        read_write = SHUT_RDWR
      };

      bool shutdown(shutdown_how how);

      // Connect.
      bool connect(const address& addr);
      bool connect(const address::ipv4& addr);
      bool connect(const address::ipv6& addr);
      bool connect(const address::local& addr);
      bool connect(const address& addr, int timeout);
      bool connect(const address::ipv4& addr, int timeout);
      bool connect(const address::ipv6& addr, int timeout);
      bool connect(const address::local& addr, int timeout);

      // Get socket error.
      bool get_socket_error(int& error);

      // Get receive buffer size.
      bool get_recvbuf_size(int& size);

      // Set receive buffer size.
      bool set_recvbuf_size(int size);

      // Get send buffer size.
      bool get_sendbuf_size(int& size);

      // Set send buffer size.
      bool set_sendbuf_size(int size);

      // Get keep-alive.
      bool get_keep_alive(bool& on);

      // Set keep-alive.
      bool set_keep_alive(bool on);

      // Get TCP no delay.
      bool get_tcp_no_delay(bool& on);

      // Set TCP no delay.
      bool set_tcp_no_delay(bool on);

      // Cork.
      bool cork();

      // Uncork.
      bool uncork();

      // Bind.
      bool bind(const address& addr);
      bool bind(const address::ipv4& addr);
      bool bind(const address::ipv6& addr);
      bool bind(const address::local& addr);

      // Listen.
      bool listen();

      // Accept.
      bool accept(socket& sock, address& addr);
      bool accept(socket& sock);
      bool accept(socket& sock, address& addr, int timeout);
      bool accept(socket& sock, int timeout);

      // Receive.
      ssize_t recv(void* buf, size_t len);
      ssize_t recv(void* buf, size_t len, int timeout);

      // Send.
      ssize_t send(const void* buf, size_t len);
      bool send(const void* buf, size_t len, int timeout);

      // Read into multiple buffers.
      ssize_t readv(const struct iovec* iov, unsigned iovcnt);
      ssize_t readv(const struct iovec* iov, unsigned iovcnt, int timeout);

      // Write from multiple buffers.
      ssize_t writev(const struct iovec* iov, unsigned iovcnt);
      bool writev(const struct iovec* iov, unsigned iovcnt, int timeout);

      // Receive from.
      ssize_t recvfrom(void* buf, size_t len, address& addr);
      ssize_t recvfrom(void* buf, size_t len);
      ssize_t recvfrom(void* buf, size_t len, address& addr, int timeout);
      ssize_t recvfrom(void* buf, size_t len, int timeout);

      // Send to.
      ssize_t sendto(const void* buf, size_t len, const address& addr);
      ssize_t sendto(const void* buf, size_t len, const address::ipv4& addr);
      ssize_t sendto(const void* buf, size_t len, const address::ipv6& addr);
      ssize_t sendto(const void* buf, size_t len, const address::local& addr);
      ssize_t sendto(const void* buf, size_t len);

      bool sendto(const void* buf,
                  size_t len,
                  const address& addr,
                  int timeout);

      bool sendto(const void* buf,
                  size_t len,
                  const address::ipv4& addr,
                  int timeout);

      bool sendto(const void* buf,
                  size_t len,
                  const address::ipv6& addr,
                  int timeout);

      bool sendto(const void* buf,
                  size_t len,
                  const address::local& addr,
                  int timeout);

      bool sendto(const void* buf, size_t len, int timeout);

      // Receive message.
      ssize_t recvmsg(struct msghdr* msg);
      ssize_t recvmsg(struct msghdr* msg, int timeout);

      // Send message.
      ssize_t sendmsg(const struct msghdr* msg);
      bool sendmsg(const struct msghdr* msg, int timeout);

#if defined(HAVE_RECVMMSG)
      // Receive multiple messages.
      int recvmmsg(struct mmsghdr* msgvec, unsigned vlen);
      int recvmmsg(struct mmsghdr* msgvec, unsigned vlen, int timeout);
#endif // defined(HAVE_RECVMMSG)

#if defined(HAVE_SENDMMSG)
      // Send multiple messages.
      int sendmmsg(struct mmsghdr* msgvec, unsigned vlen);
      bool sendmmsg(const struct mmsghdr* msgvec, unsigned vlen, int timeout);
#endif // defined(HAVE_SENDMMSG)

      // Get handle.
      handle_t handle() const;

    protected:
      handle_t _M_handle;
  };


  //////////////////////////////////////
  //////////////////////////////////////
  //                                  //
  // Class ipv4.                      //
  //                                  //
  //////////////////////////////////////
  //////////////////////////////////////

  inline socket::address::ipv4::ipv4()
  {
    _M_addr.sin_family = AF_INET;

#if !defined(__minix)
    memset(_M_addr.sin_zero, 0, sizeof(_M_addr.sin_zero));
#endif
  }

  inline socket::address::ipv4::ipv4(const ipv4& addr)
  {
    _M_addr.sin_family = AF_INET;

#if !defined(__minix)
    memset(_M_addr.sin_zero, 0, sizeof(_M_addr.sin_zero));
#endif

    *this = addr;
  }

  inline socket::address::ipv4::ipv4(const struct sockaddr_in& addr)
  {
    _M_addr.sin_family = AF_INET;

#if !defined(__minix)
    memset(_M_addr.sin_zero, 0, sizeof(_M_addr.sin_zero));
#endif

    *this = addr;
  }

  inline sa_family_t socket::address::ipv4::family()
  {
    return AF_INET;
  }

  inline in_port_t socket::address::ipv4::port() const
  {
    return ntohs(_M_addr.sin_port);
  }

  inline void socket::address::ipv4::port(in_port_t p)
  {
    _M_addr.sin_port = htons(p);
  }

  inline socklen_t socket::address::ipv4::size()
  {
    return sizeof(struct sockaddr_in);
  }

  inline bool socket::address::ipv4::build(const char* address, in_port_t port)
  {
    socklen_t addrlen;
    return internal::socket::address::build_ipv4(address,
                                                 port,
                                                 &_M_addr,
                                                 &addrlen);
  }

  inline bool socket::address::ipv4::build(const char* address)
  {
    socklen_t addrlen;
    return internal::socket::address::build_ipv4(address,
                                                 &_M_addr,
                                                 &addrlen);
  }

  inline bool socket::address::ipv4::operator==(const ipv4& addr) const
  {
    return internal::socket::address::equal(&_M_addr, &addr._M_addr);
  }

  inline
  bool socket::address::ipv4::operator==(const struct sockaddr_in& addr) const
  {
    return internal::socket::address::equal(&_M_addr, &addr);
  }

  inline
  socket::address::ipv4& socket::address::ipv4::operator=(const ipv4& addr)
  {
    _M_addr.sin_port = addr._M_addr.sin_port;
    _M_addr.sin_addr.s_addr = addr._M_addr.sin_addr.s_addr;

    return *this;
  }

  inline socket::address::ipv4&
  socket::address::ipv4::operator=(const struct sockaddr_in& addr)
  {
    _M_addr.sin_port = addr.sin_port;
    _M_addr.sin_addr.s_addr = addr.sin_addr.s_addr;

    return *this;
  }

  inline const char* socket::address::ipv4::to_string(char* s, size_t n) const
  {
    return internal::socket::address::ipv4_to_string(&_M_addr, s, n);
  }

  inline socket::address::ipv4::operator const struct sockaddr*() const
  {
    return reinterpret_cast<const struct sockaddr*>(&_M_addr);
  }

  inline socket::address::ipv4::operator struct sockaddr*()
  {
    return reinterpret_cast<struct sockaddr*>(&_M_addr);
  }

  inline socket::address::ipv4::operator const struct sockaddr&() const
  {
    return reinterpret_cast<const struct sockaddr&>(_M_addr);
  }

  inline socket::address::ipv4::operator struct sockaddr&()
  {
    return reinterpret_cast<struct sockaddr&>(_M_addr);
  }


  //////////////////////////////////////
  //////////////////////////////////////
  //                                  //
  // Class ipv6.                      //
  //                                  //
  //////////////////////////////////////
  //////////////////////////////////////

  inline socket::address::ipv6::ipv6()
  {
    _M_addr.sin6_family = AF_INET6;

    _M_addr.sin6_flowinfo = 0;
    _M_addr.sin6_scope_id = 0;
  }

  inline socket::address::ipv6::ipv6(const ipv6& addr)
  {
    _M_addr.sin6_family = AF_INET6;

    *this = addr;
  }

  inline socket::address::ipv6::ipv6(const struct sockaddr_in6& addr)
  {
    _M_addr.sin6_family = AF_INET6;

    *this = addr;
  }

  inline sa_family_t socket::address::ipv6::family()
  {
    return AF_INET6;
  }

  inline in_port_t socket::address::ipv6::port() const
  {
    return ntohs(_M_addr.sin6_port);
  }

  inline void socket::address::ipv6::port(in_port_t p)
  {
    _M_addr.sin6_port = htons(p);
  }

  inline socklen_t socket::address::ipv6::size()
  {
    return sizeof(struct sockaddr_in6);
  }

  inline bool socket::address::ipv6::build(const char* address, in_port_t port)
  {
    socklen_t addrlen;
    return internal::socket::address::build_ipv6(address,
                                                 port,
                                                 &_M_addr,
                                                 &addrlen);
  }

  inline bool socket::address::ipv6::build(const char* address)
  {
    socklen_t addrlen;
    return internal::socket::address::build_ipv6(address,
                                                 &_M_addr,
                                                 &addrlen);
  }

  inline bool socket::address::ipv6::operator==(const ipv6& addr) const
  {
    return internal::socket::address::equal(&_M_addr, &addr._M_addr);
  }

  inline
  bool socket::address::ipv6::operator==(const struct sockaddr_in6& addr) const
  {
    return internal::socket::address::equal(&_M_addr, &addr);
  }

  inline
  socket::address::ipv6& socket::address::ipv6::operator=(const ipv6& addr)
  {
    _M_addr.sin6_port = addr._M_addr.sin6_port;
    _M_addr.sin6_flowinfo = addr._M_addr.sin6_flowinfo;
    _M_addr.sin6_addr = addr._M_addr.sin6_addr;
    _M_addr.sin6_scope_id = addr._M_addr.sin6_scope_id;

    return *this;
  }

  inline socket::address::ipv6&
  socket::address::ipv6::operator=(const struct sockaddr_in6& addr)
  {
    _M_addr.sin6_port = addr.sin6_port;
    _M_addr.sin6_flowinfo = addr.sin6_flowinfo;
    _M_addr.sin6_addr = addr.sin6_addr;
    _M_addr.sin6_scope_id = addr.sin6_scope_id;

    return *this;
  }

  inline const char* socket::address::ipv6::to_string(char* s, size_t n) const
  {
    return internal::socket::address::ipv6_to_string(&_M_addr, s, n);
  }

  inline socket::address::ipv6::operator const struct sockaddr*() const
  {
    return reinterpret_cast<const struct sockaddr*>(&_M_addr);
  }

  inline socket::address::ipv6::operator struct sockaddr*()
  {
    return reinterpret_cast<struct sockaddr*>(&_M_addr);
  }

  inline socket::address::ipv6::operator const struct sockaddr&() const
  {
    return reinterpret_cast<const struct sockaddr&>(_M_addr);
  }

  inline socket::address::ipv6::operator struct sockaddr&()
  {
    return reinterpret_cast<struct sockaddr&>(_M_addr);
  }


  //////////////////////////////////////
  //////////////////////////////////////
  //                                  //
  // Class local.                     //
  //                                  //
  //////////////////////////////////////
  //////////////////////////////////////

  inline socket::address::local::local()
  {
    _M_addr.sun_family = AF_UNIX;
  }

  inline socket::address::local::local(const local& addr)
  {
    _M_addr.sun_family = AF_UNIX;

    *this = addr;
  }

  inline socket::address::local::local(const struct sockaddr_un& addr)
  {
    _M_addr.sun_family = AF_UNIX;

    *this = addr;
  }

  inline sa_family_t socket::address::local::family()
  {
    return AF_UNIX;
  }

  inline socklen_t socket::address::local::size() const
  {
    return _M_addrlen;
  }

  inline bool socket::address::local::build(const char* address)
  {
    return internal::socket::address::build_local(address,
                                                  &_M_addr,
                                                  &_M_addrlen);
  }

  inline bool socket::address::local::build(const void* address, size_t len)
  {
    return internal::socket::address::build_local(address,
                                                  len,
                                                  &_M_addr,
                                                  &_M_addrlen);
  }

  inline bool socket::address::local::operator==(const local& addr) const
  {
    return ((_M_addrlen == addr._M_addrlen) &&
            (memcmp(_M_addr.sun_path,
                    addr._M_addr.sun_path,
                    _M_addrlen - offsetof(struct sockaddr_un, sun_path)) == 0));
  }

  inline
  bool socket::address::local::operator==(const struct sockaddr_un& addr) const
  {
    return internal::socket::address::equal(&_M_addr, &addr);
  }

  inline
  socket::address::local& socket::address::local::operator=(const local& addr)
  {
    memcpy(_M_addr.sun_path,
           addr._M_addr.sun_path,
           addr._M_addrlen - offsetof(struct sockaddr_un, sun_path));

    _M_addrlen = addr._M_addrlen;

    return *this;
  }

  inline socket::address::local&
  socket::address::local::operator=(const struct sockaddr_un& addr)
  {
    internal::socket::address::build_local(&addr, &_M_addr, &_M_addrlen);

    return *this;
  }

  inline const char* socket::address::local::to_string(char* s, size_t n) const
  {
    return internal::socket::address::local_to_string(&_M_addr, s, n);
  }

  inline socket::address::local::operator const struct sockaddr*() const
  {
    return reinterpret_cast<const struct sockaddr*>(&_M_addr);
  }

  inline socket::address::local::operator struct sockaddr*()
  {
    return reinterpret_cast<struct sockaddr*>(&_M_addr);
  }

  inline socket::address::local::operator const struct sockaddr&() const
  {
    return reinterpret_cast<const struct sockaddr&>(_M_addr);
  }

  inline socket::address::local::operator struct sockaddr&()
  {
    return reinterpret_cast<struct sockaddr&>(_M_addr);
  }


  //////////////////////////////////////
  //////////////////////////////////////
  //                                  //
  // Class address.                   //
  //                                  //
  //////////////////////////////////////
  //////////////////////////////////////

  inline socket::address::address()
    : _M_addrlen(sizeof(struct sockaddr_storage))
  {
  }

  inline socket::address::address(const address& addr)
  {
    *this = addr;
  }

  inline socket::address::address(const struct sockaddr& addr)
  {
    *this = addr;
  }

  inline sa_family_t socket::address::family() const
  {
    return _M_addr.ss_family;
  }

  inline socklen_t socket::address::size() const
  {
    return _M_addrlen;
  }

  inline bool socket::address::build(const char* address, in_port_t port)
  {
    return internal::socket::address::build(
             address,
             port,
             reinterpret_cast<struct sockaddr*>(&_M_addr),
             &_M_addrlen
           );
  }

  inline bool socket::address::build(const char* address)
  {
    return internal::socket::address::build(
             address,
             reinterpret_cast<struct sockaddr*>(&_M_addr),
             &_M_addrlen
           );
  }

  inline bool socket::address::operator==(const address& addr) const
  {
    return ((_M_addrlen == addr._M_addrlen) &&
            (memcmp(&_M_addr, &addr._M_addr, _M_addrlen) == 0));
  }

  inline socket::address& socket::address::operator=(const address& addr)
  {
    memcpy(&_M_addr, &addr._M_addr, addr._M_addrlen);
    _M_addrlen = addr._M_addrlen;

    return *this;
  }

  inline const char* socket::address::to_string(char* s, size_t n) const
  {
    if (_M_addrlen > sizeof(sa_family_t)) {
      return internal::socket::address::to_string(
               reinterpret_cast<const struct sockaddr*>(&_M_addr),
               s,
               n
             );
    }

    return nullptr;
  }

  inline socket::address::operator const struct sockaddr*() const
  {
    return reinterpret_cast<const struct sockaddr*>(&_M_addr);
  }

  inline socket::address::operator struct sockaddr*()
  {
    return reinterpret_cast<struct sockaddr*>(&_M_addr);
  }

  inline socket::address::operator const struct sockaddr&() const
  {
    return reinterpret_cast<const struct sockaddr&>(_M_addr);
  }

  inline socket::address::operator struct sockaddr&()
  {
    return reinterpret_cast<struct sockaddr&>(_M_addr);
  }


  //////////////////////////////////////
  //////////////////////////////////////
  //                                  //
  // Class socket.                    //
  //                                  //
  //////////////////////////////////////
  //////////////////////////////////////

  inline socket::socket()
    : _M_handle(invalid_handle)
  {
  }

  inline socket::socket(handle_t h)
    : _M_handle(h)
  {
  }

  inline socket::~socket()
  {
    close();
  }

  inline bool socket::create(domain d, type t)
  {
    return ((_M_handle = internal::socket::create(static_cast<int>(d),
                                                  static_cast<int>(t))) !=
            invalid_handle);
  }

  inline void socket::close()
  {
    if (_M_handle != invalid_handle) {
      internal::socket::close(_M_handle);
      _M_handle = invalid_handle;
    }
  }

  inline bool socket::shutdown(shutdown_how how)
  {
    return internal::socket::shutdown(_M_handle, static_cast<int>(how));
  }

  inline bool socket::connect(const address& addr)
  {
    return internal::socket::connect(_M_handle,
                                     static_cast<const struct sockaddr*>(addr),
                                     addr.size());
  }

  inline bool socket::connect(const address::ipv4& addr)
  {
    return internal::socket::connect(_M_handle,
                                     static_cast<const struct sockaddr*>(addr),
                                     addr.size());
  }

  inline bool socket::connect(const address::ipv6& addr)
  {
    return internal::socket::connect(_M_handle,
                                     static_cast<const struct sockaddr*>(addr),
                                     addr.size());
  }

  inline bool socket::connect(const address::local& addr)
  {
    return internal::socket::connect(_M_handle,
                                     static_cast<const struct sockaddr*>(addr),
                                     addr.size());
  }

  inline bool socket::connect(const address& addr, int timeout)
  {
    return internal::socket::connect(_M_handle,
                                     static_cast<const struct sockaddr*>(addr),
                                     addr.size(),
                                     timeout);
  }

  inline bool socket::connect(const address::ipv4& addr, int timeout)
  {
    return internal::socket::connect(_M_handle,
                                     static_cast<const struct sockaddr*>(addr),
                                     addr.size(),
                                     timeout);
  }

  inline bool socket::connect(const address::ipv6& addr, int timeout)
  {
    return internal::socket::connect(_M_handle,
                                     static_cast<const struct sockaddr*>(addr),
                                     addr.size(),
                                     timeout);
  }

  inline bool socket::connect(const address::local& addr, int timeout)
  {
    return internal::socket::connect(_M_handle,
                                     static_cast<const struct sockaddr*>(addr),
                                     addr.size(),
                                     timeout);
  }

  inline bool socket::get_socket_error(int& error)
  {
    return internal::socket::get_socket_error(_M_handle, error);
  }

  inline bool socket::get_recvbuf_size(int& size)
  {
    return internal::socket::get_recvbuf_size(_M_handle, size);
  }

  inline bool socket::set_recvbuf_size(int size)
  {
    return internal::socket::set_recvbuf_size(_M_handle, size);
  }

  inline bool socket::get_sendbuf_size(int& size)
  {
    return internal::socket::get_sendbuf_size(_M_handle, size);
  }

  inline bool socket::set_sendbuf_size(int size)
  {
    return internal::socket::set_sendbuf_size(_M_handle, size);
  }

  inline bool socket::get_keep_alive(bool& on)
  {
    return internal::socket::get_keep_alive(_M_handle, on);
  }

  inline bool socket::set_keep_alive(bool on)
  {
    return internal::socket::set_keep_alive(_M_handle, on);
  }

  inline bool socket::get_tcp_no_delay(bool& on)
  {
    return internal::socket::get_tcp_no_delay(_M_handle, on);
  }

  inline bool socket::set_tcp_no_delay(bool on)
  {
    return internal::socket::set_tcp_no_delay(_M_handle, on);
  }

  inline bool socket::cork()
  {
    return internal::socket::cork(_M_handle);
  }

  inline bool socket::uncork()
  {
    return internal::socket::uncork(_M_handle);
  }

  inline bool socket::bind(const address& addr)
  {
    return internal::socket::bind(_M_handle,
                                  static_cast<const struct sockaddr*>(addr),
                                  addr.size());
  }

  inline bool socket::bind(const address::ipv4& addr)
  {
    return internal::socket::bind(_M_handle,
                                  static_cast<const struct sockaddr*>(addr),
                                  addr.size());
  }

  inline bool socket::bind(const address::ipv6& addr)
  {
    return internal::socket::bind(_M_handle,
                                  static_cast<const struct sockaddr*>(addr),
                                  addr.size());
  }

  inline bool socket::bind(const address::local& addr)
  {
    return internal::socket::bind(_M_handle,
                                  static_cast<const struct sockaddr*>(addr),
                                  addr.size());
  }

  inline bool socket::listen()
  {
    return internal::socket::listen(_M_handle);
  }

  inline bool socket::accept(socket& sock, address& addr)
  {
    return ((sock._M_handle = internal::socket::accept(
                                _M_handle,
                                static_cast<struct sockaddr*>(addr),
                                &addr._M_addrlen
                              )) != invalid_handle);
  }

  inline bool socket::accept(socket& sock)
  {
    return ((sock._M_handle = internal::socket::accept(_M_handle)) !=
            invalid_handle);
  }

  inline bool socket::accept(socket& sock, address& addr, int timeout)
  {
    return ((sock._M_handle = internal::socket::accept(
                                _M_handle,
                                static_cast<struct sockaddr*>(addr),
                                &addr._M_addrlen,
                                timeout
                              )) != invalid_handle);
  }

  inline bool socket::accept(socket& sock, int timeout)
  {
    return ((sock._M_handle = internal::socket::accept(_M_handle, timeout)) !=
            invalid_handle);
  }

  inline ssize_t socket::recv(void* buf, size_t len)
  {
    return internal::socket::recv(_M_handle, buf, len, 0);
  }

  inline ssize_t socket::recv(void* buf, size_t len, int timeout)
  {
    return internal::socket::recv(_M_handle, buf, len, 0, timeout);
  }

  inline ssize_t socket::send(const void* buf, size_t len)
  {
    return internal::socket::send(_M_handle, buf, len, 0);
  }

  inline bool socket::send(const void* buf, size_t len, int timeout)
  {
    return internal::socket::send(_M_handle, buf, len, 0, timeout);
  }

  inline ssize_t socket::readv(const struct iovec* iov, unsigned iovcnt)
  {
    return internal::socket::readv(_M_handle, iov, iovcnt);
  }

  inline ssize_t socket::readv(const struct iovec* iov,
                               unsigned iovcnt,
                               int timeout)
  {
    return internal::socket::readv(_M_handle, iov, iovcnt, timeout);
  }

  inline ssize_t socket::writev(const struct iovec* iov, unsigned iovcnt)
  {
    return internal::socket::writev(_M_handle, iov, iovcnt);
  }

  inline bool socket::writev(const struct iovec* iov,
                             unsigned iovcnt,
                             int timeout)
  {
    return internal::socket::writev(_M_handle, iov, iovcnt, timeout);
  }

  inline ssize_t socket::recvfrom(void* buf, size_t len, address& addr)
  {
    return internal::socket::recvfrom(_M_handle,
                                      buf,
                                      len,
                                      static_cast<struct sockaddr*>(addr),
                                      &addr._M_addrlen,
                                      0);
  }

  inline ssize_t socket::recvfrom(void* buf, size_t len)
  {
    return internal::socket::recvfrom(_M_handle, buf, len, nullptr, nullptr, 0);
  }

  inline ssize_t socket::recvfrom(void* buf,
                                  size_t len,
                                  address& addr,
                                  int timeout)
  {
    return internal::socket::recvfrom(_M_handle,
                                      buf,
                                      len,
                                      static_cast<struct sockaddr*>(addr),
                                      &addr._M_addrlen,
                                      0,
                                      timeout);
  }

  inline ssize_t socket::recvfrom(void* buf, size_t len, int timeout)
  {
    return internal::socket::recvfrom(_M_handle,
                                      buf,
                                      len,
                                      nullptr,
                                      nullptr,
                                      0,
                                      timeout);
  }

  inline ssize_t socket::sendto(const void* buf,
                                size_t len,
                                const address& addr)
  {
    return internal::socket::sendto(_M_handle,
                                    buf,
                                    len,
                                    static_cast<const struct sockaddr*>(addr),
                                    addr.size(),
                                    0);
  }

  inline ssize_t socket::sendto(const void* buf,
                                size_t len,
                                const address::ipv4& addr)
  {
    return internal::socket::sendto(_M_handle,
                                    buf,
                                    len,
                                    static_cast<const struct sockaddr*>(addr),
                                    addr.size(),
                                    0);
  }

  inline ssize_t socket::sendto(const void* buf,
                                size_t len,
                                const address::ipv6& addr)
  {
    return internal::socket::sendto(_M_handle,
                                    buf,
                                    len,
                                    static_cast<const struct sockaddr*>(addr),
                                    addr.size(),
                                    0);
  }

  inline ssize_t socket::sendto(const void* buf,
                                size_t len,
                                const address::local& addr)
  {
    return internal::socket::sendto(_M_handle,
                                    buf,
                                    len,
                                    static_cast<const struct sockaddr*>(addr),
                                    addr.size(),
                                    0);
  }

  inline ssize_t socket::sendto(const void* buf, size_t len)
  {
    return internal::socket::sendto(_M_handle,
                                    buf,
                                    len,
                                    nullptr,
                                    0,
                                    0);
  }

  inline bool socket::sendto(const void* buf,
                             size_t len,
                             const address& addr,
                             int timeout)
  {
    return internal::socket::sendto(_M_handle,
                                    buf,
                                    len,
                                    static_cast<const struct sockaddr*>(addr),
                                    addr.size(),
                                    0,
                                    timeout);
  }

  inline bool socket::sendto(const void* buf,
                             size_t len,
                             const address::ipv4& addr,
                             int timeout)
  {
    return internal::socket::sendto(_M_handle,
                                    buf,
                                    len,
                                    static_cast<const struct sockaddr*>(addr),
                                    addr.size(),
                                    0,
                                    timeout);
  }

  inline bool socket::sendto(const void* buf,
                             size_t len,
                             const address::ipv6& addr,
                             int timeout)
  {
    return internal::socket::sendto(_M_handle,
                                    buf,
                                    len,
                                    static_cast<const struct sockaddr*>(addr),
                                    addr.size(),
                                    0,
                                    timeout);
  }

  inline bool socket::sendto(const void* buf,
                             size_t len,
                             const address::local& addr,
                             int timeout)
  {
    return internal::socket::sendto(_M_handle,
                                    buf,
                                    len,
                                    static_cast<const struct sockaddr*>(addr),
                                    addr.size(),
                                    0,
                                    timeout);
  }

  inline bool socket::sendto(const void* buf, size_t len, int timeout)
  {
    return internal::socket::sendto(_M_handle,
                                    buf,
                                    len,
                                    nullptr,
                                    0,
                                    0,
                                    timeout);
  }

  inline ssize_t socket::recvmsg(struct msghdr* msg)
  {
    return internal::socket::recvmsg(_M_handle, msg, 0);
  }

  inline ssize_t socket::recvmsg(struct msghdr* msg, int timeout)
  {
    return internal::socket::recvmsg(_M_handle, msg, 0, timeout);
  }

  inline ssize_t socket::sendmsg(const struct msghdr* msg)
  {
    return internal::socket::sendmsg(_M_handle, msg, 0);
  }

  inline bool socket::sendmsg(const struct msghdr* msg, int timeout)
  {
    return internal::socket::sendmsg(_M_handle, msg, 0, timeout);
  }

#if defined(HAVE_RECVMMSG)
  inline int socket::recvmmsg(struct mmsghdr* msgvec, unsigned vlen)
  {
    return internal::socket::recvmmsg(_M_handle, msgvec, vlen, 0);
  }

  inline int socket::recvmmsg(struct mmsghdr* msgvec,
                              unsigned vlen,
                              int timeout)
  {
    return internal::socket::recvmmsg(_M_handle, msgvec, vlen, 0, timeout);
  }
#endif // defined(HAVE_RECVMMSG)

#if defined(HAVE_SENDMMSG)
  inline int socket::sendmmsg(struct mmsghdr* msgvec, unsigned vlen)
  {
    return internal::socket::sendmmsg(_M_handle, msgvec, vlen, 0);
  }

  inline bool socket::sendmmsg(const struct mmsghdr* msgvec,
                               unsigned vlen,
                               int timeout)
  {
    return internal::socket::sendmmsg(_M_handle, msgvec, vlen, 0, timeout);
  }
#endif // defined(HAVE_SENDMMSG)

  inline socket::handle_t socket::handle() const
  {
    return _M_handle;
  }
}

#endif // NET_SOCKET_H
