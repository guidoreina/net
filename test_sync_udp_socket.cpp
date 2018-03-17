#include <stdlib.h>
#include <stdio.h>
#include "net/sync/udp/socket.h"

static void usage(const char* program);
static int client(const char* address, const net::socket::address& addr);
static int server(const char* address, const net::socket::address& addr);

int main(int argc, const char** argv)
{
  // Check usage.
  if (argc != 3) {
    usage(argv[0]);
    return -1;
  }

  // Build socket address.
  net::socket::address addr;
  if (addr.build(argv[2])) {
    if (strcasecmp(argv[1], "--client") == 0) {
      return client(argv[2], addr);
    } else if (strcasecmp(argv[1], "--server") == 0) {
      return server(argv[2], addr);
    } else {
      usage(argv[0]);
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

int client(const char* address, const net::socket::address& addr)
{
  // Create socket.
  net::sync::udp::socket sock;
  if (sock.create(static_cast<net::socket::domain>(addr.family()))) {
    // Send data.
    if (sock.sendto("test", 4, addr, net::socket::default_timeout)) {
      printf("Data successfully sent to '%s'.\n", address);

      return 0;
    } else {
      fprintf(stderr, "Error sending data to '%s'.\n", address);
    }
  } else {
    fprintf(stderr, "Error creating socket.\n");
  }

  return -1;
}

int server(const char* address, const net::socket::address& addr)
{
  // Create socket.
  net::sync::udp::socket sock;
  if (sock.create(static_cast<net::socket::domain>(addr.family()))) {
    // Bind.
    if (sock.bind(addr)) {
      // Receive.
      net::socket::address tmpaddr;
      char buf[256];
      ssize_t ret;
      if ((ret = sock.recvfrom(buf,
                               sizeof(buf),
                               tmpaddr,
                               net::socket::default_timeout)) > 0) {
        char str[256];
        if (tmpaddr.to_string(str, sizeof(str))) {
          printf("Received data from '%s'.\n", str);
        }

        printf("Received '%.*s'.\n", static_cast<int>(ret), buf);

        return 0;
      } else {
        fprintf(stderr, "Error receiving.\n");
      }
    } else {
      fprintf(stderr, "Error binding to '%s'.\n", address);
    }
  } else {
    fprintf(stderr, "Error creating socket.\n");
  }

  return -1;
}
