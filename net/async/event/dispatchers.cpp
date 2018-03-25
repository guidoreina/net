#include "net/async/event/dispatchers.h"

#if defined(USE_SOCKET_TEMPLATE)
  #include "net/async/event/dispatcher.cpp"
#endif

#if defined(USE_SOCKET_TEMPLATE)
  template<typename T>
#endif
bool net::async::event::dispatchers::start(size_t ndispatchers)
{
  if (ndispatchers <= max_dispatchers) {
    if (ndispatchers == 0) {
      // Create dispatcher.
      if (_M_dispatchers[0].create()) {
#if defined(USE_SOCKET_TEMPLATE)
        _M_dispatchers[0].run<T>();
#else
        _M_dispatchers[0].run();
#endif
      }
    } else {
      for (_M_ndispatchers = 0;
           _M_ndispatchers < ndispatchers;
           _M_ndispatchers++) {
        // Create and start dispatcher.
        if ((!_M_dispatchers[_M_ndispatchers].create()) ||
#if defined(USE_SOCKET_TEMPLATE)
            (!_M_dispatchers[_M_ndispatchers].start<T>())) {
#else
            (!_M_dispatchers[_M_ndispatchers].start())) {
#endif
          // Stop dispatchers.
          stop();

          return false;
        }
      }
    }

    return true;
  } else {
    return false;
  }
}
