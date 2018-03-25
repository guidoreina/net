#ifndef NET_ASYNC_EVENT_DISPATCHER_H
#define NET_ASYNC_EVENT_DISPATCHER_H

#include <stdint.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/time.h>
#include "net/internal/selector.h"
#include "net/event/event.h"
#include "util/node.h"

#if !defined(USE_SOCKET_TEMPLATE)
  #define T socket
#endif

namespace net {
  namespace async {
    namespace event {
#if !defined(USE_SOCKET_TEMPLATE)
      // Forward declaration.
      class socket;
#endif // !defined(USE_SOCKET_TEMPLATE)

      class dispatcher {
        public:
          // Constructor.
          dispatcher();

          // Destructor.
          ~dispatcher();

          // Create.
          bool create();

          // Get time.
          uint64_t time() const;

          // Start.
#if defined(USE_SOCKET_TEMPLATE)
          template<typename T>
#endif
          bool start();

          // Stop.
          void stop();

          // Run.
#if defined(USE_SOCKET_TEMPLATE)
          template<typename T>
#endif
          void run();

          // Register socket without timeout.
#if defined(USE_SOCKET_TEMPLATE)
          template<typename T>
#endif
          bool register_socket(T* sock, net::event::watch ev);

          // Register socket with timeout.
          // After 'timeout' milliseconds of inactivity in the socket, the
          // method socket::timeout() will be called.
          // This method is called from socket::accept(), which is running in
          // the thread context of dispatcher::run().
#if defined(USE_SOCKET_TEMPLATE)
          template<typename T>
#endif
          bool register_socket(T* sock, net::event::watch ev, unsigned timeout);

          // Register socket with timeout.
          // The timeout has been set in sock->_M_timeout.
          // After 'timeout' milliseconds of inactivity in the socket, the
          // method socket::timeout() will be called.
          // This method is called from a thread not running the
          // dispatcher::run() method.
          // The address of the socket is written to a pipe which is read from
          // the thread running the dispatcher::run() method.
#if defined(USE_SOCKET_TEMPLATE)
          template<typename T>
#endif
          bool register_socket(T* sock);

        private:
          static const int timeout = 500; // Milliseconds.

          net::internal::selector _M_selector;

          int _M_pipe[2];

          util::node _M_header;

          struct timeval _M_start;

          // Milliseconds since start.
          uint64_t _M_time;

          pthread_t _M_thread;
          bool _M_running;

          // Run.
#if defined(USE_SOCKET_TEMPLATE)
          template<typename T>
#endif
          static void* run(void* arg);

          // Process socket.
#if defined(USE_SOCKET_TEMPLATE)
          template<typename T>
#endif
          bool process_socket(T* sock, net::event::result ev);

          // Process pipe.
#if defined(USE_SOCKET_TEMPLATE)
          template<typename T>
#endif
          void process_pipe();

          // Clear socket.
#if defined(USE_SOCKET_TEMPLATE)
          template<typename T>
#endif
          void clear_socket(T* sock);

          // Add node.
#if defined(USE_SOCKET_TEMPLATE)
          template<typename T>
#endif
          void add_node(T* sock);

          // Unlink node.
#if defined(USE_SOCKET_TEMPLATE)
          template<typename T>
#endif
          void unlink_node(T* sock);

          // Check expired sockets.
#if defined(USE_SOCKET_TEMPLATE)
          template<typename T>
#endif
          void check_expired();

          // Compute timeout.
#if defined(USE_SOCKET_TEMPLATE)
          template<typename T>
#endif
          int compute_timeout();

          // Update time.
          void update_time();
      };

      inline dispatcher::dispatcher()
        : _M_running(false)
      {
        _M_pipe[0] = -1;
        _M_pipe[1] = -1;

        _M_header.prev = &_M_header;
        _M_header.next = &_M_header;
      }

      inline dispatcher::~dispatcher()
      {
        stop();

        if (_M_pipe[0] != -1) {
          close(_M_pipe[0]);
        }

        if (_M_pipe[1] != -1) {
          close(_M_pipe[1]);
        }
      }

      inline bool dispatcher::create()
      {
        return ((_M_selector.create()) &&
                (pipe2(_M_pipe, O_NONBLOCK) == 0) &&
                (_M_selector.add(_M_pipe[0],
                                 net::event::watch::read,
                                 reinterpret_cast<void*>(_M_pipe[0]))));
      }

      inline uint64_t dispatcher::time() const
      {
        return _M_time;
      }

#if defined(USE_SOCKET_TEMPLATE)
      template<typename T>
#endif
      inline bool dispatcher::start()
      {
        _M_running = true;

        // Create thread.
#if defined(USE_SOCKET_TEMPLATE)
        if (pthread_create(&_M_thread, nullptr, run<T>, this) == 0) {
#else
        if (pthread_create(&_M_thread, nullptr, run, this) == 0) {
#endif
          return true;
        }

        _M_running = false;

        return false;
      }

      inline void dispatcher::stop()
      {
        if (_M_running) {
          _M_running = false;
          pthread_join(_M_thread, nullptr);
        }
      }

#if defined(USE_SOCKET_TEMPLATE)
      template<typename T>
#endif
      inline bool dispatcher::register_socket(T* sock)
      {
        return (write(_M_pipe[1], &sock, sizeof(T*)) ==
                static_cast<ssize_t>(sizeof(T*)));
      }

#if defined(USE_SOCKET_TEMPLATE)
      template<typename T>
#endif
      inline void* dispatcher::run(void* arg)
      {
#if defined(USE_SOCKET_TEMPLATE)
        static_cast<dispatcher*>(arg)->run<T>();
#else
        static_cast<dispatcher*>(arg)->run();
#endif

        return nullptr;
      }

      inline void dispatcher::update_time()
      {
        struct timeval now;
        gettimeofday(&now, nullptr);

        struct timeval res;
        timersub(&now, &_M_start, &res);

        _M_time = (res.tv_sec * 1000) + (res.tv_usec / 1000);
      }
    }
  }
}

#if !defined(USE_SOCKET_TEMPLATE)
  #undef T
#endif

#endif // NET_ASYNC_EVENT_DISPATCHER_H
