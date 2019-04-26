# WebRPC Roadmap

## feature list

- [x] parse sampleserver commandline arguments via program_options: specify ip, port number
- [x] parse sampleclient commandline arguments via program_options: specify host, port number, request
- [x] use parse_uri from sampleclient
- [x] add verbose option to sampleclient (show http request details only in verbose mode)
- [x] add verbose option to sampleserver (show http request / reply details in verbose mode)
- [x] cleanup: move uri decoding code from server to parse_uri
- [ ] enable parse_value to parse empty strings
- [ ] add tree representation for struct serialization
- [ ] refactor Target to SimpleTarget, which represents targets of the form `method/optional<value_t>`
- [ ] create StructTarget class to parse targets as struct_t like `{method:name,args:value_t}`
- [ ] support websocket server/client
- [ ] create sample chat server/client
- [ ] add cmake option WITH_TESTS=n to disable unit tests
- [x] add sampleclient
- [x] add sample method `DateTime`
- [x] find an alternative to CMAKE_CXX_FLAGS to enable warnings
- [x] add polymorphic value_t::get<>() to remove boost::get<>() from sample server impl
- [x] extend version info with git details and add to sampleserver and sampleclient
