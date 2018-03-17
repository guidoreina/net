#ifndef NET_INTERNAL_BSD_SELECTOR_H
#define NET_INTERNAL_BSD_SELECTOR_H

#include <unistd.h>
#include <time.h>
#include <sys/event.h>
#include "net/event/event.h"

namespace net {
  namespace internal {
    class selector {
      public:
        static const size_t max_events = 1024;

        // Constructor.
        selector();

        // Destructor.
        ~selector();

        // Create.
        bool create();

        // Add.
        bool add(int fd, event::watch ev, void* data);

        // Remove.
        bool remove(int fd, event::watch ev);

        // Modify.
        bool modify(int fd, event::watch oldev, event::watch newev, void* data);

        // Wait.
        int wait(int timeout);

        // Get result event.
        void get(size_t i, net::event::result& ev, void*& data) const;

      private:
        int _M_fd;

        struct kevent _M_events[max_events];
    };

    inline selector::selector()
      : _M_fd(-1)
    {
    }

    inline selector::~selector()
    {
      if (_M_fd != -1) {
        close(_M_fd);
      }
    }

    inline bool selector::create()
    {
      // Create event queue.
      return ((_M_fd = kqueue()) != -1);
    }

    inline int selector::wait(int timeout)
    {
      struct timespec ts = {timeout / 1000, (timeout % 1000) * 1000000};
      return kevent(_M_fd, nullptr, 0, _M_events, max_events, &ts);
    }

    inline void selector::get(size_t i,
                              net::event::result& ev,
                              void*& data) const
    {
      if ((_M_events[i].flags & (EV_ERROR | EV_EOF)) == 0) {
        switch (_M_events[i].filter) {
          case EVFILT_READ:
            ev.readable = true;
            break;
          case EVFILT_WRITE:
            ev.writable = true;
            break;
        }
      } else {
        ev.error = true;
      }

      data = reinterpret_cast<void*>(_M_events[i].udata);
    }
  }
}

#endif // NET_INTERNAL_BSD_SELECTOR_H
