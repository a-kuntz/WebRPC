# WebRPC

WebRPC implements the idea of providing a call to a remote library as a simple call to an URL. In other words, it is a web based remote procedure call (RPC) library.

## Key features

- make your remote library functionality accessible through the abstraction of a simple URL
    - call a remote library from your browser via http(s) (and soon via websocket)
    - WebRPC provides a JSON RPC inspired transfer encoding
- simple build
    - modern and clean C++ 17 code
    - cmake based build
    - little dependencies (Boost and GTest only)

## Sample Session

E.g. a remote method `DateTime` may be called via `http://localhost/DateTime` from any software that is capable of emitting http requests. For instance, this can be your browser or the WebRPC sampleclient.

Start sampleserver which is located in the build folder

    $ ./build/sampleserver

Call method `DateTime` through URL `http://localhost:8080/DateTime`. You can use your browser or sampleclient, which is also located in the build folder

    $ ./build/sampleclient http://localhost:8080/DateTime

Call method `Sum` with an array of integer and double values `[1,2.3,4e-5,-6]`

    $ ./build/sampleclient http://localhost:8080/Sum/[1,2.3,4e-5,-6]
    -2.69996

Introspection allows to list the available methods of a server

    $ ./build/sampleclient http://localhost:8080/system.list_methods

## Cloning and building WebRPC

    git clone https://github.com/a-kuntz/WebRPC.git
    cd WebRPC

    cmake -Bbuild -H.
    make -C build all test

## Dependencies

### Libraries

The productive code of the library depends only on Boost libraries. WebRPC unit tests depend on Google Test. Thus WebRPC is lightweight in terms of library dependencies.

- Boost (v 1.69.0)
  - Boost.Spirit X3
  - Boost.Asio
  - Boost.Filesystem
  - Boost.Program_options
- GTest (v. 1.8.1) - for unit tests

### Build Tools

WebRPC is build using `cmake`. Dependencies are managed by `conan` and are installed automatically at cmake configure time. WebRPC compiles and is tested on the following platforms and compilers:

| OS            | Compiler           |
| ------------- | ------------------ |
| Ubuntu 18.04  | gcc                |
| macOS 10.13.x | clang-1000.11.45.5 |

