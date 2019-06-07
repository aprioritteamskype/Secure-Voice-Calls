cmake_minimum_required(VERSION 3.1)

project(GRPC)

#--------------------------------------------------
# This branch assumes that gRPC and all its dependencies are already installed
# on this system, so they can be located by find_package().

# Find Protobuf installation
# Looks for protobuf-config.cmake file installed by Protobuf's cmake installation.
set(protobuf_MODULE_COMPATIBLE TRUE)
find_package(Protobuf REQUIRED)
message(STATUS "Using protobuf ${protobuf_VERSION}")

set(_PROTOBUF_LIBPROTOBUF protobuf::libprotobuf)
set(_PROTOBUF_PROTOC $<TARGET_FILE:protobuf::protoc>)

# Find gRPC installation
# Looks for gRPCConfig.cmake file installed by gRPC's cmake installation.

find_package(gRPC CONFIG REQUIRED)
find_package(protoc CONFIG REQUIRED)
message(STATUS "Using gRPC ${gRPC_VERSION}")

set(_GRPC_GRPCPP_UNSECURE gRPC::grpc++)
set(_GRPC_CPP_PLUGIN_EXECUTABLE $<TARGET_FILE:gRPC::grpc_cpp_plugin>)

# Proto file
get_filename_component(hw_proto "../Common/client-server.proto" ABSOLUTE)
get_filename_component(hw_proto_path "${hw_proto}" PATH)

# Generated sources
set(hw_proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/client-server.pb.cc")
set(hw_proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/client-server.pb.h")
set(hw_grpc_srcs "${CMAKE_CURRENT_BINARY_DIR}/client-server.grpc.pb.cc")
set(hw_grpc_hdrs "${CMAKE_CURRENT_BINARY_DIR}/client-server.grpc.pb.h")
add_custom_command(
      OUTPUT "${hw_proto_srcs}" "${hw_proto_hdrs}" "${hw_grpc_srcs}" "${hw_grpc_hdrs}"
      COMMAND ${_PROTOBUF_PROTOC}
      ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
        --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
        -I "${hw_proto_path}"
        --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
        "${hw_proto}"
      DEPENDS "${hw_proto}")

include_directories("${CMAKE_CURRENT_BINARY_DIR}")

add_library(${PROJECT_NAME}_lib STATIC ${hw_proto_srcs} ${hw_grpc_srcs})
add_library(SVC::GRPC ALIAS ${PROJECT_NAME}_lib)
target_include_directories(${PROJECT_NAME}_lib PUBLIC "${CMAKE_CURRENT_BINARY_DIR}")
target_link_libraries(${PROJECT_NAME}_lib PUBLIC CONAN_PKG::grpc CONAN_PKG::protobuf)
