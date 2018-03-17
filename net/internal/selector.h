#ifndef NET_INTERNAL_SELECTOR_H
#define NET_INTERNAL_SELECTOR_H

#if defined(__linux__)
  #include "net/internal/linux/selector.h"
#elif defined(__FreeBSD__) || \
      defined(__NetBSD__) || \
      defined(__OpenBSD__) || \
      defined(__DragonFly__)
  #include "net/internal/bsd/selector.h"
#endif

#endif // NET_INTERNAL_SELECTOR_H
