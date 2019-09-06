cmake_minimum_required(VERSION 3.10)

project(WebRPC)
include(GNUInstallDirs)

## options and global settings ################################################

option(WEBRPC_COVERAGE_ANALYSIS "enable coverage analysis" OFF)
message(STATUS WEBRPC_COVERAGE_ANALYSIS=${WEBRPC_COVERAGE_ANALYSIS})

option(WEBRPC_ADDRESS_SANITIZER "enable address sanitizer" ON)
message(STATUS WEBRPC_ADDRESS_SANITIZER=${WEBRPC_ADDRESS_SANITIZER})

set(BUILD_SHARED_LIBS OFF)
message(STATUS BUILD_SHARED_LIBS=${BUILD_SHARED_LIBS})

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Debug)
endif()

## configuration ##############################################################

add_compile_options(-Wall -pedantic -Wextra -Werror -Wno-error=unused-variable)

include_directories(${PROJECT_BINARY_DIR}/lib/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib/include)

if(WEBRPC_COVERAGE_ANALYSIS)
	#add_compile_options(-g -O0 -fprofile-arcs -ftest-coverage)
	#add_link_options(-fprofile-arcs -ftest-coverage)
endif()

if(WEBRPC_ADDRESS_SANITIZER)
	#add_compile_options(-g -fsanitize=address -fno-omit-frame-pointer)
	#add_link_options(-fsanitize=address -fno-omit-frame-pointer)
endif()

enable_testing()

# get version string from git tag
execute_process(
	COMMAND git describe --tags --always
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
	OUTPUT_VARIABLE WEBRPC_VERSION_STRING
	OUTPUT_STRIP_TRAILING_WHITESPACE
)

STRING(REGEX MATCH "[0-9]+\.[0-9]+\.[0-9]+" WEBRPC_VERSION ${WEBRPC_VERSION_STRING})
STRING(REGEX MATCH "[0-9]+\.[0-9]+" WEBRPC_SOVERSION ${WEBRPC_VERSION_STRING})

message(STATUS WEBRPC_VERSION_STRING=${WEBRPC_VERSION_STRING})
message(STATUS WEBRPC_VERSION=${WEBRPC_VERSION})
message(STATUS WEBRPC_SOVERSION=${WEBRPC_SOVERSION})

configure_file(
	${PROJECT_SOURCE_DIR}/lib/include/webrpc/Version.h.in
	${PROJECT_BINARY_DIR}/lib/include/webrpc/Version.h
)

## dependencies ###############################################################

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)

find_package(Conan)

conan_cmake_run(REQUIRES
	boost/1.69.0@conan/stable
	gtest/1.8.1@bincrafters/stable
	BASIC_SETUP
	BUILD missing
	OPTIONS boost:shared=False
	IMPORTS
		"bin, *.dll -> ./bin" # Copies all dll files from packages bin folder to my "bin" folder
		"lib, *.dylib* -> ./bin" # Copies all dylib files from packages lib folder to my "bin" folder
		"lib, *.so* -> ./bin" # Copies all dylib files from packages lib folder to my "bin" folder
)

find_package(Boost REQUIRED COMPONENTS filesystem program_options)
find_package(Gtest REQUIRED)

## sources ####################################################################

add_subdirectory(lib)
add_subdirectory(examples)
add_subdirectory(test)

## install targets ############################################################

# install(
# 	TARGETS webrpcdata webrpcclient webrpcserver sampleserver sampleclient
# 	EXPORT WebRPCTargets
# 	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
# 	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
# 	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
# 	# INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
# )

# install(
# 	DIRECTORY lib/include/
# 	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
# 	FILES_MATCHING PATTERN "*.h"
# )

# install(
# 	EXPORT WebRPCTargets
# 	FILE WebRPCTargets.cmake
# 	NAMESPACE WebRPC::
# 	DESTINATION ${CMAKE_INSTALL_DATADIR}/cmake/WebRPC
# )

# include(CMakePackageConfigHelpers)
# write_basic_package_version_file("WebRPCConfigVersion.cmake"
# 	VERSION ${WEBRPC_VERSION}
# 	COMPATIBILITY SameMajorVersion
# )

# install(FILES "WebRPCConfig.cmake" "${CMAKE_BINARY_DIR}/WebRPCConfigVersion.cmake"
# 	DESTINATION lib/cmake/WebRPC
# )

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
