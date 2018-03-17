#include "net/internal/bsd/selector.h"

bool net::internal::selector::add(int fd, event::watch ev, void* data)
{
  struct kevent events[2];
  unsigned nevents = 0;

  switch (ev) {
    case event::watch::read:
#if !defined(__NetBSD__)
      EV_SET(&events[0], fd, EVFILT_READ, EV_ADD | EV_CLEAR, 0, 0, data);
#else
      EV_SET(&events[0],
             fd,
             EVFILT_READ,
             EV_ADD | EV_CLEAR,
             0,
             0,
             reinterpret_cast<intptr_t>(data));
#endif

      nevents = 1;
      break;
    case event::watch::write:
#if !defined(__NetBSD__)
      EV_SET(&events[0], fd, EVFILT_WRITE, EV_ADD | EV_CLEAR, 0, 0, data);
#else
      EV_SET(&events[0],
             fd,
             EVFILT_WRITE,
             EV_ADD | EV_CLEAR,
             0,
             0,
             reinterpret_cast<intptr_t>(data));
#endif

      nevents = 1;
      break;
    case event::watch::read_write:
#if !defined(__NetBSD__)
      EV_SET(&events[0], fd, EVFILT_READ, EV_ADD | EV_CLEAR, 0, 0, data);
      EV_SET(&events[1], fd, EVFILT_WRITE, EV_ADD | EV_CLEAR, 0, 0, data);
#else
      EV_SET(&events[0],
             fd,
             EVFILT_READ,
             EV_ADD | EV_CLEAR,
             0,
             0,
             reinterpret_cast<intptr_t>(data));

      EV_SET(&events[1],
             fd,
             EVFILT_WRITE,
             EV_ADD | EV_CLEAR,
             0,
             0,
             reinterpret_cast<intptr_t>(data));
#endif

      nevents = 2;
      break;
  }

  return (kevent(_M_fd, events, nevents, nullptr, 0, nullptr) == 0);
}

bool net::internal::selector::remove(int fd, event::watch ev)
{
  struct kevent events[2];
  unsigned nevents = 0;

  switch (ev) {
    case event::watch::read:
#if !defined(__NetBSD__)
      EV_SET(&events[0], fd, EVFILT_READ, EV_DELETE, 0, 0, nullptr);
#else
      EV_SET(&events[0], fd, EVFILT_READ, EV_DELETE, 0, 0, 0);
#endif

      nevents = 1;
      break;
    case event::watch::write:
#if !defined(__NetBSD__)
      EV_SET(&events[0], fd, EVFILT_WRITE, EV_DELETE, 0, 0, nullptr);
#else
      EV_SET(&events[0], fd, EVFILT_WRITE, EV_DELETE, 0, 0, 0);
#endif

      nevents = 1;
      break;
    case event::watch::read_write:
#if !defined(__NetBSD__)
      EV_SET(&events[0], fd, EVFILT_READ, EV_DELETE, 0, 0, nullptr);
      EV_SET(&events[1], fd, EVFILT_WRITE, EV_DELETE, 0, 0, nullptr);
#else
      EV_SET(&events[0], fd, EVFILT_READ, EV_DELETE, 0, 0, 0);
      EV_SET(&events[1], fd, EVFILT_WRITE, EV_DELETE, 0, 0, 0);
#endif

      nevents = 2;
      break;
  }

  return (kevent(_M_fd, events, nevents, nullptr, 0, nullptr) == 0);
}

bool net::internal::selector::modify(int fd,
                                     event::watch oldev,
                                     event::watch newev,
                                     void* data)
{
  if (oldev == newev) {
    return add(fd, newev, data);
  }

  struct kevent events[4];
  unsigned nevents = 0;

  switch (oldev) {
    case event::watch::read:
#if !defined(__NetBSD__)
      EV_SET(&events[0], fd, EVFILT_READ, EV_DELETE, 0, 0, nullptr);
#else
      EV_SET(&events[0], fd, EVFILT_READ, EV_DELETE, 0, 0, 0);
#endif

      nevents = 1;
      break;
    case event::watch::write:
#if !defined(__NetBSD__)
      EV_SET(&events[0], fd, EVFILT_WRITE, EV_DELETE, 0, 0, nullptr);
#else
      EV_SET(&events[0], fd, EVFILT_WRITE, EV_DELETE, 0, 0, 0);
#endif

      nevents = 1;
      break;
    case event::watch::read_write:
#if !defined(__NetBSD__)
      EV_SET(&events[0], fd, EVFILT_READ, EV_DELETE, 0, 0, nullptr);
      EV_SET(&events[1], fd, EVFILT_WRITE, EV_DELETE, 0, 0, nullptr);
#else
      EV_SET(&events[0], fd, EVFILT_READ, EV_DELETE, 0, 0, 0);
      EV_SET(&events[1], fd, EVFILT_WRITE, EV_DELETE, 0, 0, 0);
#endif

      nevents = 2;
      break;
  }

  switch (newev) {
    case event::watch::read:
#if !defined(__NetBSD__)
      EV_SET(&events[nevents++],
             fd,
             EVFILT_READ,
             EV_ADD | EV_CLEAR,
             0,
             0,
             data);
#else
      EV_SET(&events[nevents++],
             fd,
             EVFILT_READ,
             EV_ADD | EV_CLEAR,
             0,
             0,
             reinterpret_cast<intptr_t>(data));
#endif

      break;
    case event::watch::write:
#if !defined(__NetBSD__)
      EV_SET(&events[nevents++],
             fd,
             EVFILT_WRITE,
             EV_ADD | EV_CLEAR,
             0,
             0,
             data);
#else
      EV_SET(&events[nevents++],
             fd,
             EVFILT_WRITE,
             EV_ADD | EV_CLEAR,
             0,
             0,
             reinterpret_cast<intptr_t>(data));
#endif

      break;
    case event::watch::read_write:
#if !defined(__NetBSD__)
      EV_SET(&events[nevents++],
             fd,
             EVFILT_READ,
             EV_ADD | EV_CLEAR,
             0,
             0,
             data);

      EV_SET(&events[nevents++],
             fd,
             EVFILT_WRITE,
             EV_ADD | EV_CLEAR,
             0,
             0,
             data);
#else
      EV_SET(&events[nevents++],
             fd,
             EVFILT_READ,
             EV_ADD | EV_CLEAR,
             0,
             0,
             reinterpret_cast<intptr_t>(data));

      EV_SET(&events[nevents++],
             fd,
             EVFILT_WRITE,
             EV_ADD | EV_CLEAR,
             0,
             0,
             reinterpret_cast<intptr_t>(data));
#endif

      break;
  }

  return (kevent(_M_fd, events, nevents, nullptr, 0, nullptr) == 0);
}
