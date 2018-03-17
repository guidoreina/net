#ifndef NET_INTERNAL_EVENT_H
#define NET_INTERNAL_EVENT_H

#include <stdint.h>
#include <poll.h>

namespace net {
  namespace internal {
    namespace event {
      enum class watch : uint32_t {
        read       = POLLIN,
        write      = POLLOUT,
        read_write = POLLIN | POLLOUT
      };
    }
  }
}

#endif // NET_INTERNAL_EVENT_H
