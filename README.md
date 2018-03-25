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

## `net::async::event::dispatcher`
* The class `net::async::event::dispatcher` can be used for monitoring I/O socket events.
* The monitored sockets are subclasses of `net::async::event::socket`.
* A timeout can be passed as parameter to the socket methods to have the dispatcher call the socket's timeout handler when the timeout has expired and no data has been transferred.
* This class has a method `run()` which waits for I/O socket events and invokes the sockets' handlers.

## `net::async::event::dispatchers`
* List of dispatchers.

## `net::async::event::socket`
* Asynchronous socket associated with a dispatcher.

## Preprocessor macro `USE_SOCKET_TEMPLATE`
* If you don't want to have virtual methods in the socket class to avoid virtual methods being called, activate this macro in the Makefile and check `test_event_template.cpp` and `Makefile.test_event_template`.
* An example using virtual methods can bee seen in `test_event.cpp` and `Makefile.test_event`.
