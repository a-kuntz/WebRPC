cmake_minimum_required(VERSION 3.10)

project(WebRPC)

## dependencies ###############################################################

# set(BOOST_ROOT "~/deps/boost_1_69_0")
find_package(Boost REQUIRED)
find_package(Threads REQUIRED)

# set(GTEST_ROOT "~/deps/googletest-release-1.8.1/build")
enable_testing()
find_package(GTest REQUIRED)

## global settings ############################################################

set(WEBRPC_VERSION_MAJOR 0)
set(WEBRPC_VERSION_MINOR 0)
set(WEBRPC_VERSION_PATCH 1)

configure_file (
	${PROJECT_SOURCE_DIR}/include/webrpc/Version.h.in
	${PROJECT_BINARY_DIR}/include/webrpc/Version.h
)

add_compile_options(-Wall -pedantic -Wextra -Werror -Wno-error=unused-variable)
# add_compile_definitions()
include_directories(${PROJECT_BINARY_DIR}/include)

## Webrpc::webrpcdata #########################################################

add_library(webrpcdata
	src/Parser.cpp
	src/Value.cpp
)

target_link_libraries(webrpcdata PUBLIC Boost::boost)

target_include_directories(webrpcdata PUBLIC ${PROJECT_SOURCE_DIR}/include)

target_compile_features(webrpcdata
	PUBLIC
		cxx_alias_templates
)

## Webrpc::webrpcserver #######################################################

add_library(webrpcserver
	src/AbstractMethod.cpp
	src/Server.cpp
)

target_link_libraries(webrpcserver Boost::boost Threads::Threads webrpcdata)

target_include_directories(webrpcserver PUBLIC ${PROJECT_SOURCE_DIR}/include)

target_compile_features(webrpcserver
	PUBLIC
		cxx_deleted_functions
	PRIVATE
		cxx_nonstatic_member_init
		cxx_std_14	# std::make_unique<.>()
)

## sample-server ##############################################################

add_executable(sampleserver
	examples/SampleServer.cpp
)

target_link_libraries(sampleserver Boost::boost webrpcserver)

target_compile_features(sampleserver
	PRIVATE
		cxx_std_14	# std::make_unique<.>()
)

## sample-parser ##############################################################

add_executable(sampleparser
	examples/SampleParser.cpp
)

target_link_libraries(sampleparser PRIVATE webrpcdata)

## map-parser-example #########################################################

add_executable(map-parser-example
	examples/map-parser-example.cpp
)

target_link_libraries(map-parser-example PRIVATE Boost::boost)

target_compile_features(map-parser-example
	PRIVATE
		cxx_std_17
)

## tests ######################################################################

add_executable(parsertest
	test/ParserTest.cpp
)

target_link_libraries(parsertest GTest::GTest GTest::Main Boost::boost webrpcdata)

# add include since parser is header only
target_include_directories(parsertest PRIVATE ${PROJECT_SOURCE_DIR}/include)

target_compile_features(parsertest
	PRIVATE
		cxx_variadic_macros
)

add_test(NAME parsertest COMMAND parsertest)

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
