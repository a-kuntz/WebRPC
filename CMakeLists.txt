cmake_minimum_required(VERSION 3.10)

project(web-rpc)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Wextra -Werror -Wno-error=unused-variable")

# set(BOOST_ROOT "~/dev/boost_1_69_0")
find_package(Boost REQUIRED)

add_executable(server
	src/SampleServerMain.cpp
	src/Method.cpp
	src/Server.cpp
)

target_include_directories(server PRIVATE "./include")

target_link_libraries(server Boost::boost)

target_compile_features(server
	# PRIVATE cxx_std_17
	PRIVATE cxx_std_14	# std::make_unique<.>()
	PRIVATE cxx_nonstatic_member_init
)
