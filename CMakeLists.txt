cmake_minimum_required(VERSION 3.10)

project(web-rpc)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Wextra -Werror -Wno-error=unused-variable")

## dependencies ###############################################################

# set(BOOST_ROOT "~/dev/boost_1_69_0")
find_package(Boost REQUIRED)

## Webrpc::webrpcvalue ########################################################

# add_library(webrpcvalue)

## Webrpc::webrpcparser #######################################################

# add_library()(webrpcparser
add_executable(webrpcparser
	src/SampleParserMain.cpp
)

target_link_libraries(webrpcparser Boost::boost)

target_include_directories(webrpcparser PUBLIC "./include")

target_compile_features(webrpcparser
	PUBLIC
		cxx_deleted_functions
		cxx_return_type_deduction
	PRIVATE
		cxx_nonstatic_member_init
)

## Webrpc::webrpcserver #######################################################

add_library(webrpcserver
	src/Method.cpp
	src/Server.cpp
)

target_link_libraries(webrpcserver Boost::boost)

target_include_directories(webrpcserver PUBLIC "./include")

target_compile_features(webrpcserver
	PUBLIC
		cxx_deleted_functions
	PRIVATE
		cxx_nonstatic_member_init
)

## sample-server ##############################################################

add_executable(sample-server
	src/SampleServerMain.cpp
)

target_link_libraries(sample-server Boost::boost webrpcserver)

target_compile_features(sample-server
	PRIVATE
		cxx_std_14	# std::make_unique<.>()
)

#install(TARGETS webrpcserver EXPORT webrpcserver-targets
#	LIBRARY DESTINATION lib
#	ARCHIVE DESTINATION lib
#	RUNTIME DESTINATION bin
#	INCLUDES DESTINATION include
#)
#install(EXPORT webrpcserver-targets
#	FILE webrpcserver-targets.cmake
#	NAMESPACE Webrpc::
#	DESTINATION lib/cmake/Webrpc
#)
