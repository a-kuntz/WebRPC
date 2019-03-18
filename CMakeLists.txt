cmake_minimum_required(VERSION 3.10)

project(WebRPC)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Wextra -Werror -Wno-error=unused-variable")

## dependencies ###############################################################

# set(BOOST_ROOT "~/dev/boost_1_69_0")
find_package(Boost REQUIRED)

# set(GTEST_ROOT "~/dev/googletest-release-1.8.1/build")
enable_testing()
find_package(Gtest REQUIRED)

## version info ###############################################################

set(WEBRPC_VERSION_MAJOR 0)
set(WEBRPC_VERSION_MINOR 0)
set(WEBRPC_VERSION_PATCH 1)

configure_file (
  "${PROJECT_SOURCE_DIR}/include/webrpc/Version.h.in"
  "${PROJECT_BINARY_DIR}/include/webrpc/Version.h"
)

include_directories("${PROJECT_BINARY_DIR}/include")

## Webrpc::webrpcvalue ########################################################

# add_library(webrpcvalue)

## Webrpc::webrpcparser #######################################################

# add_library()(webrpcparser
add_executable(webrpcparser
	examples/SampleParser.cpp
)

target_link_libraries(webrpcparser Boost::boost)

target_include_directories(webrpcparser PUBLIC "${PROJECT_SOURCE_DIR}/include")

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

target_include_directories(webrpcserver PUBLIC "${PROJECT_SOURCE_DIR}/include")

target_compile_features(webrpcserver
	PUBLIC
		cxx_deleted_functions
	PRIVATE
		cxx_nonstatic_member_init
)

## sample-server ##############################################################

add_executable(sample-server
	examples/SampleServer.cpp
)

target_link_libraries(sample-server Boost::boost webrpcserver)

target_compile_features(sample-server
	PRIVATE
		cxx_std_14	# std::make_unique<.>()
)

## tests ############################################################

add_executable(testparser
	test/ParserTest.cpp
)

target_link_libraries(testparser GTest::GTest GTest::Main Boost::boost)

# add include since parser is header only
target_include_directories(testparser PRIVATE "${PROJECT_SOURCE_DIR}/include")

target_compile_features(testparser
	PRIVATE
		cxx_variadic_macros
)

add_test(NAME testparser COMMAND testparser)

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
