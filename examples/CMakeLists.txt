## sample-server ##############################################################

add_executable(sampleserver
	src/SampleServer.cpp
)

target_link_libraries(sampleserver PRIVATE webrpcserver CONAN_PKG::boost)

target_compile_features(sampleserver PRIVATE cxx_std_14)

## sample-client ##############################################################

add_executable(sampleclient
	src/SampleClient.cpp
)

target_link_libraries(sampleclient PRIVATE webrpcclient CONAN_PKG::boost)

target_compile_features(sampleclient PRIVATE cxx_std_14)

## sample-parser ##############################################################

add_executable(sampleparser
	src/SampleParser.cpp
)

target_link_libraries(sampleparser PRIVATE webrpcdata)

