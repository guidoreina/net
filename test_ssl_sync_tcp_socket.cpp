#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "net/sync/tcp/socket.h"
#include "net/ssl/library.h"
#include "net/ssl/sync/tcp/socket.h"

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
  // Initialize SSL library.
  net::ssl::library library;
  if (!library.init(net::ssl::version::SSLv23,
                    net::ssl::thread_support::enabled)) {
    fprintf(stderr, "Error initializing SSL library.\n");
    return -1;
  }

  // Connect.
  net::sync::tcp::socket sock;
  if (!sock.connect(addr, net::socket::default_timeout)) {
    fprintf(stderr, "Error connecting to '%s'.\n", address);
    return -1;
  }

  printf("Connected to '%s'.\n", address);

  // Send data.
  if (!sock.send("unencrypted text", 16, net::socket::default_timeout)) {
    fprintf(stderr, "Error sending plain text data.\n");
    return -1;
  }

  printf("Plain text data successfully sent.\n");

  // Wait for the server to receive the data.
  sleep(1);

  net::ssl::sync::tcp::socket ssl_socket(sock);

  // Perform TLS/SSL handshake.
  if (!ssl_socket.handshake(net::ssl::socket::mode::client,
                            net::socket::default_timeout)) {
    fprintf(stderr, "Error performing TLS/SSL handshake.\n");
    return -1;
  }

  printf("Performed TLS/SSL handshake.\n");

  if (!ssl_socket.send("encrypted text", 14, net::socket::default_timeout)) {
    fprintf(stderr, "Error sending encrypted data.\n");
    return -1;
  }

  printf("Encrypted data successfully sent.\n");

  // Shutdown TLS/SSL connection.
  if (!ssl_socket.shutdown(net::ssl::socket::shutdown_how::bidirectional,
                           net::socket::default_timeout)) {
    fprintf(stderr, "Error shutting down TLS/SSL connection.\n");
    return -1;
  }

  sock.handle(ssl_socket.handle());
  ssl_socket.clear();

  if (!sock.send("unencrypted text", 16, net::socket::default_timeout)) {
    fprintf(stderr, "Error sending plain text data after encrypted data.\n");
    return -1;
  }

  printf("Plain text data successfully sent.\n");

  return 0;
}

int server(const char* address, const net::socket::address& addr)
{
  // Initialize SSL library.
  net::ssl::library library;
  if (!library.init(net::ssl::version::SSLv23,
                    net::ssl::thread_support::enabled)) {
    fprintf(stderr, "Error initializing SSL library.\n");
    return -1;
  }

  // Load certificate.
  const char* const certificate = "cert.pem";
  if (!library.load_certificate(certificate)) {
    fprintf(stderr, "Error loading certificate '%s'.\n", certificate);
    return -1;
  }

  // Load private key.
  const char* const private_key = "key.pem";
  if (!library.load_private_key(private_key)) {
    fprintf(stderr, "Error loading private key '%s'.\n", private_key);
    return -1;
  }

  // Listen.
  net::sync::tcp::socket listener;
  if (!listener.listen(addr)) {
    fprintf(stderr, "Error listening on '%s'.\n", address);
    return -1;
  }

  printf("Listening on '%s'.\n", address);

  // Accept connection.
  net::sync::tcp::socket client;
  net::socket::address clientaddr;
  if (!listener.accept(client, clientaddr, net::socket::default_timeout)) {
    fprintf(stderr, "Error accepting connection.\n");
    return -1;
  }

  char str[256];
  if (clientaddr.to_string(str, sizeof(str))) {
    printf("Accepted connection from '%s'.\n", str);
  }

  // Receive.
  ssize_t ret;
  if ((ret = client.recv(str,
                         sizeof(str),
                         net::socket::default_timeout)) <= 0) {
    fprintf(stderr, "Error receiving plain text data.\n");
    return -1;
  }

  printf("Received '%.*s'.\n", static_cast<int>(ret), str);

  net::ssl::sync::tcp::socket ssl_socket(client);

  // Perform TLS/SSL handshake.
  if (!ssl_socket.handshake(net::ssl::socket::mode::server,
                            net::socket::default_timeout)) {
    fprintf(stderr, "Error performing TLS/SSL handshake.\n");
    return -1;
  }

  // Receive.
  if ((ret = ssl_socket.recv(str,
                             sizeof(str),
                             net::socket::default_timeout)) <= 0) {
    fprintf(stderr, "Error receiving encrypted data.\n");
    return -1;
  }

  printf("Received '%.*s'.\n", static_cast<int>(ret), str);

  // Shutdown TLS/SSL connection.
  if (!ssl_socket.shutdown(net::ssl::socket::shutdown_how::bidirectional,
                           net::socket::default_timeout)) {
    fprintf(stderr, "Error shutting down TLS/SSL connection.\n");
    return -1;
  }

  client.handle(ssl_socket.handle());
  ssl_socket.clear();

  // Receive.
  if ((ret = client.recv(str,
                         sizeof(str),
                         net::socket::default_timeout)) <= 0) {
    fprintf(stderr, "Error receiving plain text data after encrypted data.\n");
    return -1;
  }

  printf("Received '%.*s'.\n", static_cast<int>(ret), str);

  return 0;
}
