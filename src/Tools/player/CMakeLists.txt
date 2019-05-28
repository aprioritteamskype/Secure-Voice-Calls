cmake_minimum_required(VERSION 2.8.12)

project(player)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")

find_package(Qt5Core)
find_package(Qt5Multimedia REQUIRED)

set(CPPSOURCES "player.cpp")
set(CPPHEADERS "player.h")


add_executable(${PROJECT_NAME} ${CPPSOURCES} ${CPPHEADERS} )
target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Multimedia)
