#ifndef NET_EVENT_H
#define NET_EVENT_H

#if defined(__linux__)
  #include "net/internal/linux/event/event.h"
#elif defined(__FreeBSD__) || \
      defined(__NetBSD__) || \
      defined(__OpenBSD__) || \
      defined(__DragonFly__)
  #include "net/internal/bsd/event/event.h"
#endif

namespace net {
  namespace event {
    typedef internal::event::watch watch;

    struct result {
      uint32_t readable:1;
      uint32_t writable:1;
      uint32_t error:1;

      // Constructor.
      result();
    };

    inline result::result()
      : readable(0),
        writable(0),
        error(0)
    {
    }
  }
}

#endif // NET_EVENT_H
