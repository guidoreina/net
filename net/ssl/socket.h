#ifndef NET_SSL_SOCKET_H
#define NET_SSL_SOCKET_H

#include "net/internal/ssl/openssl.h"

namespace net {
  namespace ssl {
    namespace socket {
      typedef internal::ssl::socket::mode mode;
      typedef internal::ssl::socket::shutdown_how shutdown_how;
    }
  }
}

#endif // NET_SSL_SOCKET_H
