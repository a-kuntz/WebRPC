## tests ######################################################################

add_executable(parsertest
	src/ParserTest.cpp
)

target_link_libraries(parsertest PRIVATE webrpcdata CONAN_PKG::gtest CONAN_PKG::boost)

target_compile_features(parsertest PRIVATE cxx_variadic_macros)

add_test(NAME parsertest COMMAND parsertest)

if(WEBRPC_COVERAGE)
add_custom_target(coverage
    COMMAND ${CMAKE_MAKE_PROGRAM} test
    COMMAND lcov --capture --directory . --output-file coverage.info
    COMMAND genhtml coverage.info --output-directory coverage
    COMMAND echo "Coverage report written to ${CMAKE_BINARY_DIR}/coverage/index.htlm"
    DEPENDS parsertest
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)
endif()