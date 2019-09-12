## tests ######################################################################

add_executable(webrpctest
	src/TestMain.cpp
	src/TestAllocator.cpp
	src/TestRegistry.cpp
	src/TestParser.cpp
	src/Testcases.cpp
	src/TestHttpWorker.cpp
	src/TestClientServer.cpp
)

target_link_libraries(webrpctest PRIVATE webrpcdata webrpcserver webrpcclient)
target_link_libraries(webrpctest PRIVATE GTest::Main)
target_link_libraries(webrpctest PRIVATE Boost::boost)

target_include_directories(webrpctest PRIVATE include)

target_compile_features(webrpctest PRIVATE cxx_variadic_macros)

add_test(NAME webrpctest COMMAND webrpctest)

set_tests_properties(webrpctest
	PROPERTIES
	ENVIRONMENT LD_LIBRARY_PATH=${CMAKE_BINARY_DIR}/lib:$ENV{LD_LIBRARY_PATH}
)

## approvals ##################################################################

add_executable(approvewebrpc
	src/ApproveMain.cpp
	src/ApproveClientServer.cpp
)

target_link_libraries(approvewebrpc PRIVATE webrpcdata webrpcserver webrpcclient)
target_link_libraries(approvewebrpc PRIVATE ApprovalTests)
target_link_libraries(approvewebrpc PRIVATE GTest::Main)
target_link_libraries(approvewebrpc PRIVATE Boost::boost)

target_include_directories(approvewebrpc PRIVATE include)

target_compile_features(approvewebrpc PRIVATE cxx_variadic_macros)

add_test(NAME approvewebrpc COMMAND approvewebrpc)

set_tests_properties(approvewebrpc
	PROPERTIES
	ENVIRONMENT LD_LIBRARY_PATH=${CMAKE_BINARY_DIR}/lib:$ENV{LD_LIBRARY_PATH}
)

## coverage ###################################################################

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