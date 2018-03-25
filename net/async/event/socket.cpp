#include "net/async/event/socket.h"

#if defined(USE_SOCKET_TEMPLATE)
  #include "net/async/event/dispatcher.cpp"
#else
  #include "net/async/event/dispatcher.inl"
#endif

#if !defined(USE_SOCKET_TEMPLATE)
  #define T socket
#endif

ssize_t net::async::event::socket::readv(const struct iovec* iov,
                                         unsigned iovcnt)
{
  // Compute how many bytes should be received.
  size_t len = 0;
  for (unsigned i = 0; i < iovcnt; i++) {
    len += iov[i].iov_len;
  }

  ssize_t ret;
  if ((ret = _M_socket.readv(iov, iovcnt)) == static_cast<ssize_t>(len)) {
    _M_timestamp = _M_dispatcher->time();
  } else if (ret >= 0) {
    _M_readable = false;
    _M_timestamp = _M_dispatcher->time();
  } else if (errno == EAGAIN) {
    _M_readable = false;
  } else {
    _M_error = true;
  }

  return ret;
}

ssize_t net::async::event::socket::writev(const struct iovec* iov,
                                          unsigned iovcnt)
{
  // Compute how many bytes should be sent.
  size_t len = 0;
  for (unsigned i = 0; i < iovcnt; i++) {
    len += iov[i].iov_len;
  }

  ssize_t ret;
  if ((ret = _M_socket.writev(iov, iovcnt)) == static_cast<ssize_t>(len)) {
    _M_timestamp = _M_dispatcher->time();
  } else if (ret >= 0) {
    _M_writable = false;
    _M_timestamp = _M_dispatcher->time();
  } else if (errno == EAGAIN) {
    _M_writable = false;
  } else {
    _M_error = true;
  }

  return ret;
}

ssize_t net::async::event::socket::sendmsg(const struct msghdr* msg)
{
  // Compute how many bytes should be sent.
  size_t len = 0;
  for (size_t i = 0; i < static_cast<size_t>(msg->msg_iovlen); i++) {
    len += msg->msg_iov[i].iov_len;
  }

  ssize_t ret;
  if ((ret = _M_socket.sendmsg(msg)) == static_cast<ssize_t>(len)) {
    _M_timestamp = _M_dispatcher->time();
  } else if (ret >= 0) {
    _M_writable = false;
    _M_timestamp = _M_dispatcher->time();
  } else if (errno == EAGAIN) {
    _M_writable = false;
  } else {
    _M_error = true;
  }

  return ret;
}

#if !defined(USE_SOCKET_TEMPLATE)
  #undef T
#endif
