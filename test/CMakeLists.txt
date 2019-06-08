## tests ######################################################################

add_executable(parsertest
	src/ParserTest.cpp
)

target_link_libraries(parsertest PRIVATE webrpcdata CONAN_PKG::gtest CONAN_PKG::boost)

target_compile_features(parsertest PRIVATE cxx_variadic_macros)

add_test(NAME parsertest COMMAND parsertest)

