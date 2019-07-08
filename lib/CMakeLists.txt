## Webrpc::webrpcdata #########################################################

add_library(webrpcdata
	src/Parser.cpp
	src/Value.cpp
)

target_link_libraries(webrpcdata PUBLIC CONAN_PKG::boost)

target_include_directories(webrpcdata
	PUBLIC
		$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
		$<INSTALL_INTERFACE:include/webrpc>
)

target_compile_features(webrpcdata PUBLIC cxx_std_17)

## Webrpc::webrpcclient #######################################################

add_library(webrpcclient
	src/Client.cpp
)

target_link_libraries(webrpcclient PUBLIC webrpcdata)
target_link_libraries(webrpcclient PUBLIC CONAN_PKG::boost)

target_include_directories(webrpcclient
	PUBLIC
		$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
		$<INSTALL_INTERFACE:include/webrpc>
)

target_compile_features(webrpcclient PUBLIC cxx_std_17)

## Webrpc::webrpcserver #######################################################

add_library(webrpcserver
	src/AbstractMethod.cpp
	src/Server.cpp
	src/detail/HttpWorker.cpp
)

target_link_libraries(webrpcserver PUBLIC webrpcdata)
target_link_libraries(webrpcserver PUBLIC CONAN_PKG::boost)

target_include_directories(webrpcserver
	PUBLIC
		$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
		$<INSTALL_INTERFACE:include/webrpc>
)

target_compile_features(webrpcserver
	PUBLIC
		cxx_deleted_functions
	PRIVATE
		cxx_nonstatic_member_init
		cxx_std_14	# std::make_unique<.>()
)

## VERSION ####################################################################

set_target_properties(
	webrpcdata webrpcclient webrpcserver
	PROPERTIES
		VERSION ${WEBRPC_VERSION}
		SOVERSION ${WEBRPC_SOVERSION}
)

