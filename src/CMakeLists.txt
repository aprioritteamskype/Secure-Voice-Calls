cmake_minimum_required(VERSION 3.1)

project(SecureVoiceCaller)

include(cmake/config.cmake)

include(cmake/conan_setup.cmake)

include_directories("plog/include")

add_subdirectory(GRPC)
add_subdirectory(Tools)
add_subdirectory(Client)
add_subdirectory(Server)
