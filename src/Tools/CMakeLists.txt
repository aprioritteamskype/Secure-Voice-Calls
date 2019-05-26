cmake_minimum_required(VERSION 3.1)

project(Tools)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 REQUIRED COMPONENTS Core)

#executable
set(CPPSOURCES "Sources/utils.cpp"
    "Sources/commandArgsParser.cpp")
set(CPPHEADERS "Headers/utils.h"
    "Headers/commandArgsParser.h")
include_directories("Headers/")

add_library(${PROJECT_NAME}_lib STATIC ${CPPSOURCES} ${CPPHEADERS})
add_library(SVC::tools ALIAS ${PROJECT_NAME}_lib)
target_include_directories(${PROJECT_NAME}_lib PUBLIC "${PROJECT_SOURCE_DIR}/Headers")
target_link_libraries(${PROJECT_NAME}_lib Qt5::Core)
