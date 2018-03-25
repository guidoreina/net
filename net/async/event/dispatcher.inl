#ifndef NET_ASYNC_EVENT_DISPATCHER_INL
#define NET_ASYNC_EVENT_DISPATCHER_INL

#if !defined(USE_SOCKET_TEMPLATE)
  #include "net/async/event/socket.h"

  #define T socket
#endif // !defined(USE_SOCKET_TEMPLATE)

#if defined(USE_SOCKET_TEMPLATE)
  template<typename T>
  inline
  bool net::async::event::dispatcher::register_socket(T* sock,
                                                      net::event::watch ev)
  {
    return _M_selector.add(sock->handle(), ev, sock);
  }
#endif // defined(USE_SOCKET_TEMPLATE)

#if defined(USE_SOCKET_TEMPLATE)
  template<typename T>
#endif
inline bool net::async::event::dispatcher::register_socket(T* sock,
                                                           net::event::watch ev,
                                                           unsigned timeout)
{
  if (register_socket(sock, ev)) {
    sock->_M_timestamp = _M_time;
    sock->_M_timeout = timeout;
    sock->_M_expire = _M_time + timeout;

    add_node(sock);

    return true;
  }

  return false;
}

#if defined(USE_SOCKET_TEMPLATE)
  template<typename T>
#endif
inline void net::async::event::dispatcher::clear_socket(T* sock)
{
  // Close socket.
  sock->_M_socket.close();

  // Initialize socket (in case it might be reused).
  sock->init();

  // Clear socket (socket might be deleted, if wished).
  sock->clear();
}

#if defined(USE_SOCKET_TEMPLATE)
  template<typename T>
#endif
inline void net::async::event::dispatcher::unlink_node(T* sock)
{
  // Unlink node.
  if (sock->prev) {
    sock->prev->next = sock->next;
    sock->next->prev = sock->prev;

    sock->prev = nullptr;
    sock->next = nullptr;
  }
}

#if defined(USE_SOCKET_TEMPLATE)
  template<typename T>
#endif
inline int net::async::event::dispatcher::compute_timeout()
{
  // If there is at least one socket...
  if (_M_header.next != &_M_header) {
    uint64_t left =
      static_cast<T*>(_M_header.next)->_M_expire - _M_time;

    return (static_cast<int>(left) < timeout) ? static_cast<int>(left) :
                                                timeout;
  } else {
    return timeout;
  }
}

#if !defined(USE_SOCKET_TEMPLATE)
  #undef T
#endif

#endif // NET_ASYNC_EVENT_DISPATCHER_INL
