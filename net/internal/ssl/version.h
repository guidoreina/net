#ifndef NET_INTERNAL_SSL_VERSION_H
#define NET_INTERNAL_SSL_VERSION_H

namespace net {
  namespace internal {
    namespace ssl {
      enum class version {
        SSLv3,
        TLSv1,
        TLSv1_1,
        TLSv1_2,
        TLSv1_3,
        SSLv23,
        DTLSv1,
        DTLSv1_2
      };
    }
  }
}

#endif // NET_INTERNAL_SSL_VERSION_H
