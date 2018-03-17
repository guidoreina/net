# net
Socket classes.

## `net::socket`
* The class `net::socket` can be used for synchronous and asynchronous operations.
* Use the functions without timeout for asynchronous operations.
* Use the functions with timeout for synchronous operations. The timeout has to be specified in milliseconds.
* The socket is always non-blocking.

### `net::sync::socket`
The class `net::sync::socket` inherits from `net::socket` and just deletes the methods without timeout.

#### `net::sync::tcp::socket`
The class `net::sync::tcp::socket` inherits from `net::sync::socket` and can be used for synchronous stream sockets.

#### `net::sync::udp::socket`
The class `net::sync::udp::socket` inherits from `net::sync::socket` and can be used for synchronous datagram sockets.

### `net::async::socket`
The class `net::async::socket` inherits from `net::socket` and just deletes the methods with timeout.

#### `net::async::tcp::socket`
The class `net::async::tcp::socket` inherits from `net::async::socket` and can be used for asynchronous stream sockets.

#### `net::async::udp::socket`
The class `net::async::udp::socket` inherits from `net::async::socket` and can be used for asynchronous datagram sockets.
