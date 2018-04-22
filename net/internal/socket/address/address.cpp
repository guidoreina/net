#include <stddef.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <errno.h>
#include "net/internal/socket/address/address.h"

namespace net {
  namespace internal {
    namespace socket {
      namespace address {
        static bool parse_port(const char* s, in_port_t& port)
        {
          unsigned n = 0;
          while (*s) {
            if ((*s >= '0') &&
                (*s <= '9') &&
                ((n = (n * 10) + (*s - '0')) <= 65535)) {
              s++;
            } else {
              return false;
            }
          }

          if (n > 0) {
            port = static_cast<in_port_t>(n);
            return true;
          }

          return false;
        }

        bool extract_ip_port(const char* address, char* ip, in_port_t& port)
        {
          // Search last colon.
          const char* colon = nullptr;
          const char* p = address;
          while (*p) {
            if (*p == ':') {
              colon = p;
            }

            p++;
          }

          if (colon) {
            size_t len;
            if ((len = colon - address) > 0) {
              if (*address == '[') {
                if ((len > 2) && (colon[-1] == ']')) {
                  // Skip '['.
                  address++;

                  len -= 2;
                } else {
                  return false;
                }
              }

              if (len < INET6_ADDRSTRLEN) {
                if (parse_port(colon + 1, port)) {
                  memcpy(ip, address, len);
                  ip[len] = 0;

                  return true;
                }
              }
            }
          }

          return false;
        }

        bool build_ipv4(const char* address,
                        in_port_t port,
                        struct sockaddr_in* addr,
                        socklen_t* addrlen)
        {
          if (inet_pton(AF_INET, address, &addr->sin_addr) == 1) {
            addr->sin_family = AF_INET;
            addr->sin_port = htons(port);

#if !defined(__minix)
            memset(addr->sin_zero, 0, sizeof(addr->sin_zero));
#endif

            *addrlen = sizeof(struct sockaddr_in);

            return true;
          }

          return false;
        }

        bool build_ipv6(const char* address,
                        in_port_t port,
                        struct sockaddr_in6* addr,
                        socklen_t* addrlen)
        {
          if (inet_pton(AF_INET6, address, &addr->sin6_addr) == 1) {
            addr->sin6_family = AF_INET6;
            addr->sin6_port = htons(port);
            addr->sin6_flowinfo = 0;
            addr->sin6_scope_id = 0;

            *addrlen = sizeof(struct sockaddr_in6);

            return true;
          }

          return false;
        }

        bool build_local(const char* address,
                         struct sockaddr_un* addr,
                         socklen_t* addrlen)
        {
          size_t len;
          if (((len = strlen(address)) > 0) && (len < sizeof(addr->sun_path))) {
            addr->sun_family = AF_UNIX;

            memcpy(addr->sun_path, address, len);
            addr->sun_path[len] = 0;

            *addrlen = offsetof(struct sockaddr_un, sun_path) + len + 1;

#if defined(__FreeBSD__) || \
    defined(__NetBSD__) || \
    defined(__OpenBSD__) || \
    defined(__DragonFly__)
            addr->sun_len = static_cast<uint8_t>(*addrlen);
#endif

            return true;
          }

          return false;
        }

        bool build_local(const void* address,
                         size_t len,
                         struct sockaddr_un* addr,
                         socklen_t* addrlen)
        {
          if ((len > 0) && (len <= sizeof(addr->sun_path))) {
            addr->sun_family = AF_UNIX;

            memcpy(addr->sun_path, address, len);
            memset(addr->sun_path + len, 0, sizeof(addr->sun_path) - len);

            *addrlen = sizeof(struct sockaddr_un);

#if defined(__FreeBSD__) || \
    defined(__NetBSD__) || \
    defined(__OpenBSD__) || \
    defined(__DragonFly__)
            addr->sun_len = static_cast<uint8_t>(*addrlen);
#endif

            return true;
          }

          return false;
        }

        void build_local(const struct sockaddr_un* address,
                         struct sockaddr_un* addr,
                         socklen_t* addrlen)
        {
          addr->sun_family = AF_UNIX;

          // If not an abstract socket address...
          if (address->sun_path[0]) {
            const char* src = address->sun_path;
            const char* const end = src + sizeof(address->sun_path);
            char* dest = addr->sun_path;

            while (src < end) {
              *dest++ = *src;

              if (!*src++) {
                break;
              }
            }

            *addrlen = offsetof(struct sockaddr_un, sun_path) +
                       (src - address->sun_path);
          } else {
            memcpy(addr->sun_path,
                   address->sun_path,
                   sizeof(address->sun_path));

            *addrlen = sizeof(struct sockaddr_un);
          }

#if defined(__FreeBSD__) || \
    defined(__NetBSD__) || \
    defined(__OpenBSD__) || \
    defined(__DragonFly__)
          addr->sun_len = static_cast<uint8_t>(*addrlen);
#endif
        }

        bool build(const char* address,
                   struct sockaddr* addr,
                   socklen_t* addrlen)
        {
          char ip[INET6_ADDRSTRLEN];
          in_port_t port;
          if (extract_ip_port(address, ip, port)) {
            return build(ip, port, addr, addrlen);
          } else {
            return build_local(address,
                               reinterpret_cast<struct sockaddr_un*>(addr),
                               addrlen);
          }
        }

        const char* ipv4_to_string(const struct sockaddr_in* addr,
                                   char* s,
                                   size_t n)
        {
          if (inet_ntop(AF_INET, &addr->sin_addr, s, n)) {
            size_t len = strlen(s);
            size_t left = n - len;

            if (snprintf(s + len,
                         left,
                         ":%u",
                         ntohs(addr->sin_port)) <
                static_cast<ssize_t>(left)) {
              return s;
            } else {
              errno = ENOSPC;
            }
          }

          return nullptr;
        }

        const char* ipv6_to_string(const struct sockaddr_in6* addr,
                                   char* s,
                                   size_t n)
        {
          if (n > 1) {
            if (inet_ntop(AF_INET6, &addr->sin6_addr, s + 1, n - 1)) {
              size_t len = 1 + strlen(s + 1);
              size_t left = n - len;

              if (snprintf(s + len,
                           left,
                           "]:%u",
                           ntohs(addr->sin6_port)) <
                  static_cast<ssize_t>(left)) {
                *s = '[';

                return s;
              } else {
                errno = ENOSPC;
              }
            }
          } else {
            errno = ENOSPC;
          }

          return nullptr;
        }

        const char* local_to_string(const struct sockaddr_un* addr,
                                    char* s,
                                    size_t n)
        {
          size_t len;
          if ((len = strlen(addr->sun_path)) < n) {
            memcpy(s, addr->sun_path, len);
            s[len] = 0;

            return s;
          } else {
            errno = ENOSPC;
            return nullptr;
          }
        }
      }
    }
  }
}
