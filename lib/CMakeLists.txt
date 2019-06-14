## Webrpc::webrpcdata #########################################################

add_library(webrpcdata
	src/Parser.cpp
	src/Value.cpp
)

target_link_libraries(webrpcdata PUBLIC CONAN_PKG::boost)

target_include_directories(webrpcdata PUBLIC ${PROJECT_SOURCE_DIR}/include)

target_compile_features(webrpcdata PUBLIC cxx_std_17)

## Webrpc::webrpcclient #######################################################

add_library(webrpcclient
	src/Client.cpp
)

target_link_libraries(webrpcclient CONAN_PKG::boost webrpcdata)

target_include_directories(webrpcclient PUBLIC ${PROJECT_SOURCE_DIR}/include)

target_compile_features(webrpcclient PUBLIC cxx_std_17)

## Webrpc::webrpcserver #######################################################

add_library(webrpcserver
	src/AbstractMethod.cpp
	src/Server.cpp
	src/detail/HttpWorker.cpp
)

target_link_libraries(webrpcserver PUBLIC webrpcdata CONAN_PKG::boost)

target_include_directories(webrpcserver PUBLIC ${PROJECT_SOURCE_DIR}/include)

target_compile_features(webrpcserver
	PUBLIC
		cxx_deleted_functions
	PRIVATE
		cxx_nonstatic_member_init
		cxx_std_14	# std::make_unique<.>()
)

