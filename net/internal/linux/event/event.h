#ifndef NET_INTERNAL_EVENT_H
#define NET_INTERNAL_EVENT_H

#include <stdint.h>
#include <sys/epoll.h>

namespace net {
  namespace internal {
    namespace event {
      enum class watch : uint32_t {
        read       = EPOLLIN | EPOLLRDHUP | EPOLLET,
        write      = EPOLLOUT | EPOLLET,
        read_write = EPOLLIN | EPOLLRDHUP | EPOLLOUT | EPOLLET
      };
    }
  }
}

#endif // NET_INTERNAL_EVENT_H
