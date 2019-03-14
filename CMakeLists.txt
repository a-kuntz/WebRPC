cmake_minimum_required(VERSION 3.10)

project(web-rpc)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Wextra -Werror")

find_package(Boost REQUIRED)

add_executable(server
	src/ServerMain.cpp
	src/Method.cpp
	src/Registry.cpp
)

target_include_directories(server PRIVATE "./include")

target_link_libraries(server Boost::boost)

target_compile_features(server
	# PRIVATE cxx_std_17
	PRIVATE cxx_nonstatic_member_init
)