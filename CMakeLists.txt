cmake_minimum_required(VERSION 3.10)

project(WebRPC)

## dependencies ###############################################################

if(NOT CMAKE_BUILD_TYPE)
set(CMAKE_BUILD_TYPE debug)
endif()

## conan target structure
# CONAN_PKG::boost
# CONAN_LIB::<package_name>_<library_name>
# CONAN_LIB::boost_boost_program_options

# Download automatically, you can also just copy the conan.cmake file
if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
   message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
   file(DOWNLOAD "https://github.com/conan-io/cmake-conan/raw/v0.14/conan.cmake"
                 "${CMAKE_BINARY_DIR}/conan.cmake")
endif()

include(${CMAKE_BINARY_DIR}/conan.cmake)

conan_cmake_run(REQUIRES
		boost/1.69.0@conan/stable
		gtest/1.8.1@bincrafters/stable
	BASIC_SETUP CMAKE_TARGETS
	BUILD missing
)

## global settings ############################################################

enable_testing()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# get version string from tag
execute_process(
	COMMAND git describe --tags --always
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
	OUTPUT_VARIABLE WEBRPC_VERSION_STRING
	OUTPUT_STRIP_TRAILING_WHITESPACE
)

message(STATUS WEBRPC_VERSION_STRING=${WEBRPC_VERSION_STRING})

configure_file (
	${PROJECT_SOURCE_DIR}/lib/include/webrpc/Version.h.in
	${PROJECT_BINARY_DIR}/lib/include/webrpc/Version.h
)
include_directories(${PROJECT_BINARY_DIR}/lib/include)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib/include)

add_compile_options(-Wall -pedantic -Wextra -Werror -Wno-error=unused-variable -fvisibility=hidden)
#add_compile_options(-g -fsanitize=address -fno-omit-frame-pointer)
#add_compile_definitions()

## sources ####################################################################

add_subdirectory(lib)
add_subdirectory(examples)
add_subdirectory(test)

## install targets ############################################################

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

## clang-format ###############################################################

add_custom_command(
	OUTPUT clang-format
	COMMAND find . -name *.cpp -o -name *.h | xargs clang-format -i
)

add_custom_target(beautify
	DEPENDS clang-format
)

## qtcreator ##################################################################

set(FIND_OPTS -path ./deps -prune -o -path ./build -prune -o)

add_custom_command(
	OUTPUT creator
	COMMAND echo "[General]" > ${CMAKE_PROJECT_NAME}.creator
	COMMAND touch ${CMAKE_PROJECT_NAME}.config
	COMMAND echo "./include" > ${CMAKE_PROJECT_NAME}.includes
	COMMAND echo "${CONAN_BOOST_ROOT}/include" >> ${CMAKE_PROJECT_NAME}.includes
	COMMAND echo "${CONAN_GTEST_ROOT}/include" >> ${CMAKE_PROJECT_NAME}.includes
	COMMAND find . ${FIND_OPTS} -name *.cpp -o -name *.h -o -name CMakeLists.txt > ${CMAKE_PROJECT_NAME}.files
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

add_custom_target(qtcreator
	DEPENDS creator
)
