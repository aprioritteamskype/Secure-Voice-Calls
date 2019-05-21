cmake_minimum_required(VERSION 2.8.11)
project(Tests)

enable_testing()

# Tell CMake to run moc when necessary:
set(CMAKE_AUTOMOC ON)

# As moc files are generated in the binary dir, tell CMake
# to always look for includes there:
set(CMAKE_INCLUDE_CURRENT_DIR ON
    )
set(TOOLSPATH "../Tools")
include_directories("${TOOLSPATH}/recorder")
include_directories("${TOOLSPATH}/player")

find_package(Qt5Test REQUIRED)
find_package(Qt5Multimedia REQUIRED)

SET( Player_SRCS
    "../Tools/player/player.cpp")

SET( Recorder_SRCS
    "../Tools/recorder/recorder.cpp")

SET( RecorderTests_SRCS RecorderTests.cpp RecorderTests.h)#FIXME

SET( PlayerTests_SRCS PlayerTests.cpp PlayerTests.h)

add_executable(Tests ${PlayerTests_SRCS} ${Player_SRCS} ${RecorderTests_SRCS} ${Recorder_SRCS})

#add_executable(PlayerTests ${PlayerTests_SRCS} ${Player_SRCS})
add_test(NAME Tests COMMAND Tests)

#SET( RecorderTests_SRCS RecorderTests.cpp RecorderTests.h)
#add_executable(RecorderTests ${RecorderTests_SRCS} ${Recorder_SRCS})
#add_test(NAME RecorderTests COMMAND RecorderTests)

#target_link_libraries(Tests PlayerTests RecorderTests Qt5::Test Qt5::Multimedia)
target_link_libraries(Tests  Qt5::Test Qt5::Multimedia)
