#ifndef NET_INTERNAL_LINUX_SELECTOR_H
#define NET_INTERNAL_LINUX_SELECTOR_H

#include <unistd.h>
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

        struct epoll_event _M_events[max_events];
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
      // Create epoll file descriptor.
      return ((_M_fd = epoll_create1(0)) != -1);
    }

    inline bool selector::add(int fd, event::watch ev, void* data)
    {
      struct epoll_event event;
      event.events = static_cast<uint32_t>(ev);
      event.data.ptr = data;

      return (epoll_ctl(_M_fd, EPOLL_CTL_ADD, fd, &event) == 0);
    }

    inline bool selector::remove(int fd, event::watch ev)
    {
      struct epoll_event event;
      event.events = 0;
      event.data.u64 = 0;

      return (epoll_ctl(_M_fd, EPOLL_CTL_DEL, fd, &event) == 0);
    }

    inline bool selector::modify(int fd,
                                 event::watch oldev,
                                 event::watch newev,
                                 void* data)
    {
      struct epoll_event event;
      event.events = static_cast<uint32_t>(newev);
      event.data.ptr = data;

      return (epoll_ctl(_M_fd, EPOLL_CTL_MOD, fd, &event) == 0);
    }

    inline int selector::wait(int timeout)
    {
      return epoll_wait(_M_fd, _M_events, max_events, timeout);
    }

    inline void selector::get(size_t i,
                              net::event::result& ev,
                              void*& data) const
    {
      if ((_M_events[i].events & (EPOLLRDHUP | EPOLLERR | EPOLLHUP)) == 0) {
        if (_M_events[i].events & EPOLLIN) {
          ev.readable = true;
        }

        if (_M_events[i].events & EPOLLOUT) {
          ev.writable = true;
        }
      } else {
        ev.error = true;
      }

      data = _M_events[i].data.ptr;
    }
  }
}

#endif // NET_INTERNAL_LINUX_SELECTOR_H
