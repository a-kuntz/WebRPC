## tests ######################################################################

add_executable(webrpctest
	src/RegistryTest.cpp
	src/ParserTest.cpp
	src/HttpWorkerTest.cpp
	src/ClientServerTest.cpp
	src/Testcase.cpp
)

target_link_libraries(webrpctest PRIVATE webrpcdata webrpcserver webrpcclient CONAN_PKG::gtest CONAN_PKG::boost)

target_compile_features(webrpctest PRIVATE cxx_variadic_macros)

target_include_directories(webrpctest
	PRIVATE
		include
)

add_test(NAME webrpctest COMMAND webrpctest)

if(WEBRPC_COVERAGE_ANALYSIS)
add_custom_target(coverage
	COMMAND find . -name *.gcda | xargs rm
	COMMAND ${CMAKE_MAKE_PROGRAM} test
	COMMAND lcov --capture --directory . --output-file coverage.info
	COMMAND genhtml coverage.info --output-directory coverage
	COMMAND echo "Coverage report written to ${CMAKE_BINARY_DIR}/coverage/index.htlm"
	DEPENDS webrpctest
	WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)
endif()