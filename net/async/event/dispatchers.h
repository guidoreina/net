#ifndef NET_ASYNC_EVENT_DISPATCHERS_H
#define NET_ASYNC_EVENT_DISPATCHERS_H

#include "net/async/event/dispatcher.h"

namespace net {
  namespace async {
    namespace event {
      class dispatchers {
        public:
          // Constructor.
          dispatchers();

          // Destructor.
          ~dispatchers();

          // Start.
#if defined(USE_SOCKET_TEMPLATE)
          template<typename T>
#endif
          bool start(size_t ndispatchers = 0);

          // Stop.
          void stop();

          // Get dispatcher.
          dispatcher* get(size_t i);

        private:
          static const size_t max_dispatchers = 32;

          dispatcher _M_dispatchers[max_dispatchers];
          size_t _M_ndispatchers;
      };

      inline dispatchers::dispatchers()
        : _M_ndispatchers(0)
      {
      }

      inline dispatchers::~dispatchers()
      {
        stop();
      }

      inline void dispatchers::stop()
      {
        // Stop dispatchers.
        for (size_t i = 0; i < _M_ndispatchers; i++) {
          _M_dispatchers[i].stop();
        }
      }

      inline dispatcher* dispatchers::get(size_t i)
      {
        return (i < _M_ndispatchers) ? &_M_dispatchers[i] : nullptr;
      }
    }
  }
}

#endif // NET_ASYNC_EVENT_DISPATCHERS_H
