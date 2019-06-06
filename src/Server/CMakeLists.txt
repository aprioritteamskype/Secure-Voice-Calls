cmake_minimum_required(VERSION 3.1)

project(Server)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt5Core REQUIRED)

set(CPPSOURCES "main.cpp" "server.cpp")
set(CPPHEADERS "server.h")

add_executable(${PROJECT_NAME} ${CPPSOURCES} ${CPPHEADERS})

target_link_libraries(${PROJECT_NAME} Qt5::Core CONAN_PKG::grpc SVC::GRPC SVC::tools)
