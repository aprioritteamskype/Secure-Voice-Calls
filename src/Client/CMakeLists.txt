cmake_minimum_required(VERSION 3.1)

project(Client LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 COMPONENTS Core Quick REQUIRED)
find_package(Qt5Multimedia REQUIRED)

set(TOOLSPATH "../Tools")
include_directories("${TOOLSPATH}/recorder")
include_directories("${TOOLSPATH}/player")
message("paht to tools ${TOOLSPATH}")

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/Headers")

set(CPPSOURCES "Sources/main.cpp"
    "Sources/client.cpp"
    "Sources/qmlclientstate.cpp"
    "Sources/qmlclientsonlinemodel.cpp"
    "Sources/qmlmissedcallsmodel.cpp"
    "Sources/peertopeer.cpp"
    "Sources/audiomodule.cpp"
    "Sources/audiowriter.cpp"
    "Sources/audioreader.cpp")
set(CPPHEADERS "Headers/client.h"
    "Headers/qmlclientstate.h"
    "Headers/qmlclientsonlinemodel.h"
    "Headers/qmlmissedcallsmodel.h"
    "Headers/peertopeer.h"
    "Headers/audiomodule.h"
    "Headers/audiowriter.h"
    "Headers/audioreader.h")

add_executable(${PROJECT_NAME} ${CPPSOURCES} ${CPPHEADERS} "qml.qrc")
target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Quick SVC::GRPC SVC::tools)
