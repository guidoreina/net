#ifndef NET_INTERNAL_SOCKET_ADDRESS_H
#define NET_INTERNAL_SOCKET_ADDRESS_H

#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/un.h>

namespace net {
  namespace internal {
    namespace socket {
      namespace address {
        // Equal?
        static inline bool equal(const struct in_addr* addr1,
                                 const struct in_addr* addr2)
        {
          return (addr1->s_addr == addr2->s_addr);
        }

        static inline bool equal(const struct in6_addr* addr1,
                                 const struct in6_addr* addr2)
        {
#if defined(__linux__)
          return (((addr1->s6_addr32[0] ^ addr2->s6_addr32[0]) |
                   (addr1->s6_addr32[1] ^ addr2->s6_addr32[1]) |
                   (addr1->s6_addr32[2] ^ addr2->s6_addr32[2]) |
                   (addr1->s6_addr32[3] ^ addr2->s6_addr32[3])) == 0);
#elif defined(__FreeBSD__) || \
      defined(__NetBSD__) || \
      defined(__OpenBSD__) || \
      defined(__DragonFly__)
          return (((addr1->__u6_addr.__u6_addr32[0] ^
                    addr2->__u6_addr.__u6_addr32[0]) |
                   (addr1->__u6_addr.__u6_addr32[1] ^
                    addr2->__u6_addr.__u6_addr32[1]) |
                   (addr1->__u6_addr.__u6_addr32[2] ^
                    addr2->__u6_addr.__u6_addr32[2]) |
                   (addr1->__u6_addr.__u6_addr32[3] ^
                    addr2->__u6_addr.__u6_addr32[3])) == 0);
#else
          return false;
#endif
        }

        static inline bool equal(const struct sockaddr_in* addr1,
                                 const struct sockaddr_in* addr2)
        {
          return ((addr1->sin_port == addr2->sin_port) &&
                  (equal(&addr1->sin_addr, &addr2->sin_addr)));
        }

        static inline bool equal(const struct sockaddr_in6* addr1,
                                 const struct sockaddr_in6* addr2)
        {
          return ((addr1->sin6_port == addr2->sin6_port) &&
                  (equal(&addr1->sin6_addr, &addr2->sin6_addr)));
        }

        static inline bool equal(const struct sockaddr_un* addr1,
                                 const struct sockaddr_un* addr2)
        {
          return addr1->sun_path[0] ?
                                      (strncmp(addr1->sun_path,
                                               addr2->sun_path,
                                               sizeof(addr1->sun_path)) == 0) :
                                      (memcmp(addr1->sun_path,
                                              addr2->sun_path,
                                              sizeof(addr1->sun_path)) == 0);
        }

        // Extract IP and port.
        // 'ip' has to be, at least, INET6_ADDRSTRLEN bytes long.
        bool extract_ip_port(const char* address, char* ip, in_port_t& port);

        // Build socket address.
        bool build_ipv4(const char* address,
                        in_port_t port,
                        struct sockaddr_in* addr,
                        socklen_t* addrlen);

        static inline bool build_ipv4(const char* address,
                                      struct sockaddr_in* addr,
                                      socklen_t* addrlen)
        {
          char ip[INET6_ADDRSTRLEN];
          in_port_t port;
          return extract_ip_port(address, ip, port) &&
                 build_ipv4(ip, port, addr, addrlen);
        }

        bool build_ipv6(const char* address,
                        in_port_t port,
                        struct sockaddr_in6* addr,
                        socklen_t* addrlen);

        static inline bool build_ipv6(const char* address,
                                      struct sockaddr_in6* addr,
                                      socklen_t* addrlen)
        {
          char ip[INET6_ADDRSTRLEN];
          in_port_t port;
          return extract_ip_port(address, ip, port) &&
                 build_ipv6(ip, port, addr, addrlen);
        }

        bool build_local(const char* address,
                         struct sockaddr_un* addr,
                         socklen_t* addrlen);

        bool build_local(const void* address,
                         size_t len,
                         struct sockaddr_un* addr,
                         socklen_t* addrlen);

        void build_local(const struct sockaddr_un* address,
                         struct sockaddr_un* addr,
                         socklen_t* addrlen);

        static inline bool build(const char* address,
                                 in_port_t port,
                                 struct sockaddr* addr,
                                 socklen_t* addrlen)
        {
          return build_ipv4(address,
                            port,
                            reinterpret_cast<struct sockaddr_in*>(addr),
                            addrlen) ||
                 build_ipv6(address,
                            port,
                            reinterpret_cast<struct sockaddr_in6*>(addr),
                            addrlen);
        }

        bool build(const char* address,
                   struct sockaddr* addr,
                   socklen_t* addrlen);

        // To string.
        const char* ipv4_to_string(const struct sockaddr_in* addr,
                                   char* s,
                                   size_t n);

        const char* ipv6_to_string(const struct sockaddr_in6* addr,
                                   char* s,
                                   size_t n);

        const char* local_to_string(const struct sockaddr_un* addr,
                                    char* s,
                                    size_t n);

        static inline const char* to_string(const struct sockaddr* addr,
                                            char* s,
                                            size_t n)
        {
          switch (addr->sa_family) {
            case AF_INET:
              return ipv4_to_string(
                       reinterpret_cast<const struct sockaddr_in*>(addr),
                       s,
                       n
                     );
            case AF_INET6:
              return ipv6_to_string(
                       reinterpret_cast<const struct sockaddr_in6*>(addr),
                       s,
                       n
                     );
            case AF_UNIX:
              return local_to_string(
                       reinterpret_cast<const struct sockaddr_un*>(addr),
                       s,
                       n
                     );
            default:
              return nullptr;
          }
        }
      }
    }
  }
}

#endif // NET_INTERNAL_SOCKET_ADDRESS_H
