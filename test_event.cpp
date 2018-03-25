#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <signal.h>
#include <memory>
#include "net/async/event/dispatchers.h"
#include "net/async/event/socket.h"

namespace client {
  class socket : public net::async::event::socket {
    public:
      // Constructor.
      socket(net::async::event::dispatcher* dispatcher)
        : net::async::event::socket(dispatcher),
          _M_off(0),
          _M_state(0)
      {
      }

      // Destructor.
      ~socket() = default;

      // Clear.
      void clear()
      {
        printf("[client::socket::clear]\n");

        _M_off = 0;
        _M_state = 0;
      }

      // Timeout.
      bool timeout()
      {
        printf("[client::socket::timeout]\n");
        return false;
      }

      // Run.
      bool run()
      {
        //printf("[client::socket::run]\n");

        do {
          switch (_M_state) {
            case 0: // Initial state.
              {
                // If the connection succeeded...
                int error;
                if ((get_socket_error(error)) && (error == 0)) {
                  _M_state = 1;
                } else {
                  fprintf(stderr, "[client::socket::run] Error connecting.\n");
                  return false;
                }
              }

              // Fall through.
            case 1: // Sending.
              {
                static constexpr const char* const request = "GET / HTTP/1.1\r\n"
                                                             "Host: 127.0.0.1\r\n"
                                                             "Connection: close\r\n"
                                                             "\r\n";

                static constexpr const size_t requestlen = strlen(request);

                size_t left = requestlen - _M_off;

                ssize_t ret;
                if ((ret = send(request + _M_off, left)) ==
                    static_cast<ssize_t>(left)) {
                  _M_off = 0;

                  _M_state = 2; // Receiving.
                } else if (ret > 0) {
                  _M_off += ret;
                  return true;
                } else {
                  return !error();
                }
              }

              // Fall through.
            case 2: // Receiving.
              {
                size_t left = sizeof(_M_buf) - _M_off;

                ssize_t ret;
                if ((ret = recv(_M_buf + _M_off, left)) > 0) {
                  _M_off += ret;

                  if (memmem(_M_buf, _M_off, "\r\n\r\n", 4)) {
                    // Completed.
                    _M_off = 0;

                    _M_state = 1; // Sending.
                  } else {
                    if (static_cast<size_t>(ret) < left) {
                      return true;
                    }

                    // Response too long.
                    return false;
                  }
                } else if (ret == 0) {
                  // Connection closed by peer.
                  return false;
                } else {
                  return !error();
                }
              }

              break;
          }
        } while (true);
      }

    private:
      uint8_t _M_buf[4 * 1024];
      size_t _M_off;

      int _M_state;
  };
}

namespace server {
  // Forward declaration.
  class acceptor;

  class socket : public net::async::event::socket {
    friend class acceptor;

    public:
      // Constructor.
      socket(acceptor* acceptor)
        : _M_off(0),
          _M_state(0),
          _M_acceptor(acceptor),
          _M_prev(nullptr),
          _M_next(nullptr)
      {
      }

      // Destructor.
      ~socket() = default;

      // Clear.
      void clear();

      // Timeout.
      bool timeout()
      {
        printf("[server::socket::timeout]\n");
        return false;
      }

      // Run.
      bool run()
      {
        //printf("[socket::server::run]\n");

        do {
          switch (_M_state) {
            case 0: // Receiving.
              {
                size_t left = sizeof(_M_buf) - _M_off;

                ssize_t ret;
                if ((ret = recv(_M_buf + _M_off, left)) > 0) {
                  _M_off += ret;

                  if (memmem(_M_buf, _M_off, "\r\n\r\n", 4)) {
                    _M_off = 0;

                    _M_state = 1; // Sending.
                  } else {
                    if (static_cast<size_t>(ret) < left) {
                      return true;
                    }

                    // Request too long.
                    return false;
                  }
                } else if (ret == 0) {
                  // Connection closed by peer.
                  return false;
                } else {
                  return !error();
                }
              }

              // Fall through.
            case 1: // Sending.
              {
                static constexpr const char* const response =
                  "HTTP/1.1 200 OK\r\n"
                  "Date: Tue, 22 Aug 2017 16:22:50 GMT\r\n"
                  "Content-Length: 2\r\n"
                  "\r\n"
                  "OK";

                static constexpr const size_t responselen = strlen(response);

                size_t left = responselen - _M_off;

                ssize_t ret;
                if ((ret = send(response + _M_off, left)) ==
                    static_cast<ssize_t>(left)) {
                  // Completed.
                  _M_off = 0;

                  _M_state = 0; // Receiving.
                } else if (ret > 0) {
                  _M_off += ret;
                  return true;
                } else {
                  return !error();
                }
              }

              break;
          }
        } while (true);
      }

    private:
      uint8_t _M_buf[4 * 1024];
      size_t _M_off;

      int _M_state;

      acceptor* _M_acceptor;

      socket* _M_prev;
      socket* _M_next;
  };

  class acceptor : public net::async::event::socket {
    public:
      // Constructor.
      acceptor(net::async::event::dispatcher* dispatcher)
        : net::async::event::socket(dispatcher),
          _M_used_sockets(nullptr),
          _M_free_sockets(nullptr)
      {
      }

      // Destructor.
      ~acceptor()
      {
        // Delete sockets in use.
        server::socket* sock = _M_used_sockets;

        while (sock) {
          server::socket* next = sock->_M_next;

          delete sock;
          sock = next;
        }

        // Delete free sockets.
        sock = _M_free_sockets;

        while (sock) {
          server::socket* next = sock->_M_next;

          delete sock;
          sock = next;
        }
      }

      // Free server socket.
      void free_server_socket(server::socket* sock)
      {
        unlink_node(_M_used_sockets, sock);
        link_node(_M_free_sockets, sock);
      }

      // Clear.
      void clear()
      {
        printf("[server::acceptor::clear]\n");
      }

      // Timeout.
      bool timeout()
      {
        printf("[server::acceptor::timeout]\n");
        return false;
      }

      // Run.
      bool run()
      {
        printf("[server::acceptor::run]\n");

        // If there is a free server socket...
        server::socket* server;
        if (_M_free_sockets) {
          server = _M_free_sockets;
          unlink_node(_M_free_sockets, server);

          printf("[server::acceptor::run] Reusing server socket.\n");
        } else {
          if ((server = new (std::nothrow) server::socket(this)) != nullptr) {
            printf("[server::acceptor::run] Created new server socket.\n");
          } else {
            return false;
          }
        }

        net::socket::address addr;
        if (accept(*server, addr)) {
          char str[256];
          if (addr.to_string(str, sizeof(str))) {
            printf("Accepted connection from '%s'.\n", str);
          }

          link_node(_M_used_sockets, server);

          return true;
        }

        // Add socket to the free list.
        link_node(_M_free_sockets, server);

        return !error();
      }

    private:
      server::socket* _M_used_sockets;
      server::socket* _M_free_sockets;

      static void unlink_node(server::socket*& node, server::socket* sock)
      {
        if (sock->_M_prev) {
          sock->_M_prev->_M_next = sock->_M_next;
        } else {
          node = node->_M_next;
        }

        if (sock->_M_next) {
          sock->_M_next->_M_prev = sock->_M_prev;
          sock->_M_next = nullptr;
        }

        sock->_M_prev = nullptr;
      }

      static void link_node(server::socket*& node, server::socket* sock)
      {
        if (node) {
          node->_M_prev = sock;
        }

        sock->_M_prev = nullptr;
        sock->_M_next = node;

        node = sock;
      }
  };

  void socket::clear()
  {
    printf("[server::socket::clear]\n");

    _M_off = 0;
    _M_state = 0;

    _M_acceptor->free_server_socket(this);
  }
}

static const int timeout = 30 * 1000; // Milliseconds.

static void usage(const char* program);
static int run_client(const char* address,
                      const net::socket::address& addr,
                      net::async::event::dispatchers& dispatchers,
                      const sigset_t* set);

static int run_server(const char* address,
                      const net::socket::address& addr,
                      net::async::event::dispatchers& dispatchers,
                      const sigset_t* set);

int main(int argc, const char** argv)
{
  // Check usage.
  if (argc != 3) {
    usage(argv[0]);
    return -1;
  }

  enum class command {
    client,
    server
  };

  command cmd;
  if (strcasecmp(argv[1], "--client") == 0) {
    cmd = command::client;
  } else if (strcasecmp(argv[1], "--server") == 0) {
    cmd = command::server;
  } else {
    usage(argv[0]);
    return -1;
  }

  // Build socket address.
  net::socket::address addr;
  if (addr.build(argv[2])) {
    // Block signals SIGINT and SIGTERM.
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGINT);
    sigaddset(&set, SIGTERM);
    if (pthread_sigmask(SIG_BLOCK, &set, NULL) == 0) {
      // Start dispatchers.
      net::async::event::dispatchers dispatchers;

      if (dispatchers.start(1)) {
        if (cmd == command::client) {
          return run_client(argv[2], addr, dispatchers, &set);
        } else {
          return run_server(argv[2], addr, dispatchers, &set);
        }
      } else {
        fprintf(stderr, "Error starting dispatchers.\n");
      }
    } else {
      fprintf(stderr, "Error blocking signals SIGINT and SIGTERM.\n");
    }
  } else {
    fprintf(stderr, "Invalid address '%s'.\n", argv[2]);
  }

  return -1;
}

void usage(const char* program)
{
  fprintf(stderr, "Usage: %s --client|--server <address>\n", program);
}

int run_client(const char* address,
               const net::socket::address& addr,
               net::async::event::dispatchers& dispatchers,
               const sigset_t* set)
{
  client::socket sock(dispatchers.get(0));

  // Connect.
  if (sock.connect(addr, timeout)) {
    // Wait for signal to arrive.
    int sig;
    while (sigwait(set, &sig) != 0);

    dispatchers.stop();

    printf("Exiting...\n");

    return 0;
  } else {
    fprintf(stderr, "Error connecting to '%s'.\n", address);
  }

  return -1;
}

int run_server(const char* address,
               const net::socket::address& addr,
               net::async::event::dispatchers& dispatchers,
               const sigset_t* set)
{
  server::acceptor sock(dispatchers.get(0));

  // Listen.
  if (sock.listen(addr)) {
    printf("Listening on '%s'.\n", address);

    // Wait for signal to arrive.
    int sig;
    while (sigwait(set, &sig) != 0);

    dispatchers.stop();

    printf("Exiting...\n");

    return 0;
  } else {
    fprintf(stderr, "Error listening on '%s'.\n", address);
  }

  return -1;
}
