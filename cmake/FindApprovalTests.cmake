if(NOT TARGET ApprovalTests)
	set(APPROVAL_TEST_INCLUDE_DIR ${CMAKE_BINARY_DIR}/include)
	if(NOT EXISTS "${APPROVAL_TEST_INCLUDE_DIR}/ApprovalTests.hpp")
		message(STATUS "Downloading ApprovalTests.hpp")
		file(DOWNLOAD "https://github.com/approvals/ApprovalTests.cpp/releases/download/v.4.0.0/ApprovalTests.v.4.0.0.hpp" "${APPROVAL_TEST_INCLUDE_DIR}/ApprovalTests.hpp")
	endif()

	find_package(GTest REQUIRED)

	add_library(ApprovalTests INTERFACE)
	target_include_directories(ApprovalTests INTERFACE ${APPROVAL_TEST_INCLUDE_DIR})
	target_link_libraries(ApprovalTests INTERFACE GTest::GTest GTest::Main)
endif()