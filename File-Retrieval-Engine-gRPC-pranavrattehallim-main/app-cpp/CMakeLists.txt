cmake_minimum_required(VERSION 3.22)

project(sockets
        VERSION 1.0
        DESCRIPTION "C++ gRPC File Retrieval Engine"
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(PkgConfig)
pkg_search_module(GRPC REQUIRED grpc++)

find_package(Protobuf REQUIRED)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_library(FileRetrievalEngine proto/File-Retrieval-Engine.proto)
target_link_libraries(FileRetrievalEngine PUBLIC ${GRPC_LIBRARIES} ${Protobuf_LIBRARIES})
protobuf_generate(TARGET FileRetrievalEngine)
protobuf_generate(
    TARGET FileRetrievalEngine
    LANGUAGE grpc
    PLUGIN protoc-gen-grpc=/usr/bin/grpc_cpp_plugin
    GENERATE_EXTENSIONS .grpc.pb.h .grpc.pb.cc
)

add_executable(file-retrieval-server
               src/file-retrieval-server.cpp
               src/ServerAppInterface.cpp
               src/ServerProcessingEngine.cpp
               src/IndexStore.cpp
               src/FileRetrievalEngineImpl.cpp)
target_include_directories(file-retrieval-server PUBLIC include)
target_link_libraries(file-retrieval-server FileRetrievalEngine)

add_executable(file-retrieval-client
               src/file-retrieval-client.cpp
               src/ClientAppInterface.cpp
               src/ClientProcessingEngine.cpp)
target_include_directories(file-retrieval-client PUBLIC include)
target_link_libraries(file-retrieval-client FileRetrievalEngine)

# Add the benchmark executable
add_executable(file-retrieval-benchmark
               src/file-retrieval-benchmark.cpp
               src/ClientProcessingEngine.cpp) # Include ClientProcessingEngine for benchmark

target_include_directories(file-retrieval-benchmark PUBLIC include)
target_link_libraries(file-retrieval-benchmark FileRetrievalEngine)

# Now set the include directories for both executables
target_include_directories(file-retrieval-server PUBLIC include ${CMAKE_CURRENT_BINARY_DIR}/proto)
target_include_directories(file-retrieval-client PUBLIC include ${CMAKE_CURRENT_BINARY_DIR}/proto)
target_include_directories(file-retrieval-benchmark PUBLIC include ${CMAKE_CURRENT_BINARY_DIR}/proto)

# Link the library to the executables
target_link_libraries(file-retrieval-server FileRetrievalEngine)
target_link_libraries(file-retrieval-client FileRetrievalEngine)
target_link_libraries(file-retrieval-benchmark FileRetrievalEngine)


