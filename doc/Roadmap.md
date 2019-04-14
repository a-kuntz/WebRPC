# WebRPC Roadmap

## feature list

- [x] parse sampleserver commandline arguments via program_options: specify ip, port number
- [x] parse sampleclient commandline arguments via program_options: specify host, port number, request
- [x] use parse_uri from sampleclient
- [x] add verbose option to sampleclient (show http request details only in verbose mode)
- [ ] add tree representation for struct serialization
- [ ] create request class to parse requests as struct_t
- [ ] support websocket server/client
- [ ] create sample chat server/client
- [ ] add cmake option WITH_TESTS=n to disable unit tests
- [x] add sampleclient
- [x] add sample method `DateTime`
- [x] find an alternative to CMAKE_CXX_FLAGS to enable warnings
- [x] add polymorphic value_t::get<>() to remove boost::get<>() from sample server impl
- [x] extend version info with git details and add to sampleserver and sampleclient
