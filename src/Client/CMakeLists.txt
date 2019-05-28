cmake_minimum_required(VERSION 3.1)

project(Client LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


#conan_basic_setup()
if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
   message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
   file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/v0.8/conan.cmake"
                 "${CMAKE_BINARY_DIR}/conan.cmake")
endif()

include(${CMAKE_BINARY_DIR}/conan.cmake)
conan_cmake_run(REQUIRES grpc/1.20.0@inexorgame/stable
                BASIC_SETUP Client
                BUILD missing)

find_package(Qt5 COMPONENTS Core Quick REQUIRED)
find_package(Qt5Multimedia REQUIRED)
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

 set(TOOLSPATH "../Tools")
 include_directories("${TOOLSPATH}/recorder")
 include_directories("${TOOLSPATH}/player")
 message("paht to tools ${TOOLSPATH}")

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/Headers")
# Include generated *.pb.h files
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

set(CPPSOURCES "Sources/main.cpp"
    "Sources/client.cpp"
    "Sources/qmlclientstate.cpp"
    "Sources/qmlclientsonlinemodel.cpp"
    "Sources/peertopeer.cpp"
    "Sources/audiomodule.cpp"
    "Sources/audiowriter.cpp"
    "Sources/audioreader.cpp")
set(CPPHEADERS "Headers/client.h"
    "Headers/qmlclientstate.h"
    "Headers/qmlclientsonlinemodel.h"
    "Headers/peertopeer.h"
    "Headers/audiomodule.h"
    "Headers/audiowriter.h"
    "Headers/audioreader.h")

add_executable(${PROJECT_NAME} ${CPPSOURCES} ${CPPHEADERS} "qml.qrc" ${hw_proto_srcs} ${hw_grpc_srcs})
target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Quick ${CONAN_LIBS} SVC::tools)
