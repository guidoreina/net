#include "net/async/event/dispatcher.h"
#include "net/async/event/dispatcher.inl"

#if !defined(USE_SOCKET_TEMPLATE)
  #define T socket
#endif

#if !defined(USE_SOCKET_TEMPLATE)
  bool net::async::event::dispatcher::register_socket(T* sock,
                                                      net::event::watch ev)
  {
    return _M_selector.add(sock->handle(), ev, sock);
  }
#endif // !defined(USE_SOCKET_TEMPLATE)

#if defined(USE_SOCKET_TEMPLATE)
  template<typename T>
#endif
void net::async::event::dispatcher::run()
{
  // Get current time.
  gettimeofday(&_M_start, nullptr);
  _M_time = 0;

  do {
    // Wait for events.
#if defined(USE_SOCKET_TEMPLATE)
    int ret = _M_selector.wait(compute_timeout<T>());
#else
    int ret = _M_selector.wait(compute_timeout());
#endif

    // Update time.
    update_time();

    T* errors[net::internal::selector::max_events];
    size_t nerrors = 0;

    // Process events.
    for (int i = 0; i < ret; i++) {
      net::event::result ev;
      T* sock;
      _M_selector.get(i, ev, reinterpret_cast<void*&>(sock));

      // If the event is not for the pipe...
      if (reinterpret_cast<uintptr_t>(sock) !=
          static_cast<uintptr_t>(_M_pipe[0])) {
        if (!ev.error) {
          if (!sock->_M_error) {
            // Process socket.
            if (!process_socket(sock, ev)) {
              // Socket failed.
              sock->_M_error = true;
              errors[nerrors++] = sock;
            }
          }
        } else if (!sock->_M_error) {
          // Socket failed.
          sock->_M_error = true;
          errors[nerrors++] = sock;
        }
      } else if (ev.readable) {
        // Process pipe.
#if defined(USE_SOCKET_TEMPLATE)
        process_pipe<T>();
#else
        process_pipe();
#endif
      }
    }

    // Clear failed sockets.
    for (size_t i = 0; i < nerrors; i++) {
      // Unlink node.
      unlink_node(errors[i]);

      // Clear socket.
      clear_socket(errors[i]);
    }

    // Check expired sockets.
#if defined(USE_SOCKET_TEMPLATE)
    check_expired<T>();
#else
    check_expired();
#endif
  } while (_M_running);
}

#if defined(USE_SOCKET_TEMPLATE)
  template<typename T>
#endif
bool net::async::event::dispatcher::process_socket(T* sock,
                                                   net::event::result ev)
{
  sock->_M_readable |= ev.readable;
  sock->_M_writable |= ev.writable;

  uint64_t oldtimestamp = sock->_M_timestamp;
  int oldtimeout = sock->_M_timeout;

  if (sock->run()) {
    if (sock->_M_timeout >= 0) {
      if ((oldtimestamp != sock->_M_timestamp) ||
          (oldtimeout != sock->_M_timeout)) {
        // Unlink node.
        unlink_node(sock);

        sock->_M_expire = sock->_M_timestamp + sock->_M_timeout;

        add_node(sock);
      }
    } else {
      // Unlink node.
      unlink_node(sock);
    }

    return true;
  } else {
    // Socket failed.
    return false;
  }
}

#if defined(USE_SOCKET_TEMPLATE)
  template<typename T>
#endif
void net::async::event::dispatcher::process_pipe()
{
  T* sock;
  while (read(_M_pipe[0], &sock, sizeof(T*)) == sizeof(T*)) {
    if (register_socket(sock, sock->_M_event)) {
      sock->_M_timestamp = _M_time;
      sock->_M_expire = _M_time + sock->_M_timeout;

      add_node(sock);
    } else {
      // Clear socket.
      clear_socket(sock);
    }
  }
}

#if defined(USE_SOCKET_TEMPLATE)
  template<typename T>
#endif
void net::async::event::dispatcher::add_node(T* sock)
{
  uint64_t expire = sock->_M_expire;

  util::node* s = _M_header.prev;

  while ((s != &_M_header) && (expire < static_cast<T*>(s)->_M_expire)) {
    s = s->prev;
  }

  sock->prev = s;
  sock->next = s->next;

  sock->next->prev = sock;
  s->next = sock;
}

#if defined(USE_SOCKET_TEMPLATE)
  template<typename T>
#endif
void net::async::event::dispatcher::check_expired()
{
  util::node* s = _M_header.next;

  while ((s != &_M_header) && (_M_time >= static_cast<T*>(s)->_M_expire)) {
    util::node* next = s->next;

    if (!static_cast<T*>(s)->timeout()) {
      // Unlink node.
      unlink_node(static_cast<T*>(s));

      // Clear socket.
      clear_socket(static_cast<T*>(s));

      s = next;
    } else {
      s = s->next;
    }
  }
}

#if !defined(USE_SOCKET_TEMPLATE)
  #undef T
#endif
