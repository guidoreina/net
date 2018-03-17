#include <stdlib.h>
#include <stdio.h>
#include "net/sync/tcp/socket.h"

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
  // Connect.
  net::sync::tcp::socket sock;
  if (sock.connect(addr, net::socket::default_timeout)) {
    printf("Connected to '%s'.\n", address);

    // Send data.
    if (sock.send("test", 4, net::socket::default_timeout)) {
      printf("Data successfully sent.\n");

      return 0;
    } else {
      fprintf(stderr, "Error sending data.\n");
    }
  } else {
    fprintf(stderr, "Error connecting to '%s'.\n", address);
  }

  return -1;
}

int server(const char* address, const net::socket::address& addr)
{
  // Listen.
  net::sync::tcp::socket listener;
  if (listener.listen(addr)) {
    printf("Listening on '%s'.\n", address);

    // Accept connection.
    net::sync::tcp::socket client;
    net::socket::address clientaddr;
    if (listener.accept(client, clientaddr, net::socket::default_timeout)) {
      char str[256];
      if (clientaddr.to_string(str, sizeof(str))) {
        printf("Accepted connection from '%s'.\n", str);
      }

      // Receive.
      ssize_t ret;
      if ((ret = client.recv(str,
                             sizeof(str),
                             net::socket::default_timeout)) > 0) {
        printf("Received '%.*s'.\n", static_cast<int>(ret), str);

        return 0;
      } else {
        fprintf(stderr, "Error receiving.\n");
      }
    } else {
      fprintf(stderr, "Error accepting connection.\n");
    }
  } else {
    fprintf(stderr, "Error listening on '%s'.\n", address);
  }

  return -1;
}
