cmake_minimum_required(VERSION 3.10)

project(WebRPC)

## dependencies ###############################################################

# set(BOOST_ROOT "${CMAKE_SOURCE_DIR}/deps/boost_1_69_0")
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost COMPONENTS program_options filesystem REQUIRED)
find_package(Threads REQUIRED)

# set(GTEST_ROOT "${CMAKE_SOURCE_DIR}/deps/googletest-release-1.8.1/build")
enable_testing()
find_package(GTest REQUIRED)

## global settings ############################################################

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(WEBRPC_VERSION_MAJOR 0)
set(WEBRPC_VERSION_MINOR 0)
set(WEBRPC_VERSION_PATCH 1)

# Get the current working branch
execute_process(
	COMMAND git rev-parse --abbrev-ref HEAD
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
	OUTPUT_VARIABLE GIT_BRANCH
	OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Get the latest abbreviated commit hash of the working branch
execute_process(
	COMMAND git log -1 --format=%H
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
	OUTPUT_VARIABLE GIT_COMMIT_HASH
	OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Get working copy state: 0=clean 1=modified
execute_process(
	COMMAND git diff-index --quiet HEAD --
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
	RESULT_VARIABLE GIT_WORKING_COPY_MODIFIED
)


message("--- GIT_COMMIT_HASH=${GIT_COMMIT_HASH} GIT_BRANCH=${GIT_BRANCH} GIT_WORKING_COPY_MODIFIED=${GIT_WORKING_COPY_MODIFIED}")

configure_file (
	${PROJECT_SOURCE_DIR}/include/webrpc/Version.h.in
	${PROJECT_BINARY_DIR}/include/webrpc/Version.h
)

add_compile_options(-Wall -pedantic -Wextra -Werror -Wno-error=unused-variable -fvisibility=hidden)
#add_compile_options(-g -fsanitize=address -fno-omit-frame-pointer)
#add_compile_definitions()
include_directories(${PROJECT_BINARY_DIR}/include)

## Webrpc::webrpcdata #########################################################

add_library(webrpcdata
	src/Parser.cpp
	src/Value.cpp
)

target_link_libraries(webrpcdata PUBLIC Boost::boost)

target_include_directories(webrpcdata PUBLIC ${PROJECT_SOURCE_DIR}/include)

target_compile_features(webrpcdata PUBLIC cxx_std_17)

## Webrpc::webrpcclient #######################################################

add_library(webrpcclient
	src/Client.cpp
)

target_link_libraries(webrpcclient Boost::boost Threads::Threads webrpcdata)

target_include_directories(webrpcclient PUBLIC ${PROJECT_SOURCE_DIR}/include)

target_compile_features(webrpcclient PUBLIC cxx_std_17)

## Webrpc::webrpcserver #######################################################

add_library(webrpcserver
	src/AbstractMethod.cpp
	src/Server.cpp
)

target_link_libraries(webrpcserver PUBLIC webrpcdata Boost::boost Threads::Threads)

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

target_link_libraries(sampleserver PRIVATE webrpcserver Boost::boost Boost::program_options Threads::Threads)

target_compile_features(sampleserver PRIVATE cxx_std_14)

## sample-client ##############################################################

add_executable(sampleclient
	examples/SampleClient.cpp
)

target_link_libraries(sampleclient PRIVATE webrpcclient Boost::boost Boost::program_options Threads::Threads)

target_compile_features(sampleclient PRIVATE cxx_std_14)

## sample-parser ##############################################################

add_executable(sampleparser
	examples/SampleParser.cpp
)

target_link_libraries(sampleparser PRIVATE webrpcdata)

## tests ######################################################################

add_executable(parsertest
	test/ParserTest.cpp
)

target_link_libraries(parsertest PRIVATE webrpcdata GTest::GTest GTest::Main Boost::boost)

target_compile_features(parsertest PRIVATE cxx_variadic_macros)

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

add_custom_command(
	OUTPUT clang-format
	COMMAND find . -name *.cpp -o -name *.h | xargs clang-format -i
)

add_custom_target(beautify
	DEPENDS clang-format
)

set(FIND_OPTS -path ./deps -prune -o -path ./build -prune -o)

add_custom_command(
	OUTPUT creator
	COMMAND echo "[General]" > ${CMAKE_PROJECT_NAME}.creator
	COMMAND touch ${CMAKE_PROJECT_NAME}.config
	COMMAND echo "./include" > ${CMAKE_PROJECT_NAME}.includes
	COMMAND echo "${BOOST_ROOT}/include" >> ${CMAKE_PROJECT_NAME}.includes
	COMMAND echo "${GTEST_ROOT}/include" >> ${CMAKE_PROJECT_NAME}.includes
	COMMAND find . ${FIND_OPTS} -name *.cpp -o -name *.h -o -name CMakeLists.txt > ${CMAKE_PROJECT_NAME}.files
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

add_custom_target(qtcreator
	DEPENDS creator
)
