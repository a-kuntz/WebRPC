cmake_minimum_required(VERSION 3.10)

project(web-rpc)

find_package(Boost REQUIRED)

add_executable(server
	http_server_fast.cpp
	fields_alloc.hpp
)

target_link_libraries(server Boost::boost)

target_compile_features(server
	PRIVATE cxx_std_17
)