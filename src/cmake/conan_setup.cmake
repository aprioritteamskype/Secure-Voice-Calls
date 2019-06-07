include(cmake/conan.cmake)

conan_check(1.16.0)

conan_add_remote(
   NAME conan-transit
   URL https://api.bintray.com/conan/conan/conan-transit
)

conan_add_remote(
   NAME bincrafters
   URL https://api.bintray.com/conan/bincrafters/public-conan
)

conan_add_remote(
   NAME inexorgame
   URL https://api.bintray.com/conan/inexorgame/inexor-conan
)


if (IS_MULTI_CONFIG)
    # Suppressing warnings for multi generators msvc,xcode.
    set(CUSTOM_NO_OUTPUT_DIR_PARAM)
else (IS_MULTI_CONFIG)
    set(CUSTOM_NO_OUTPUT_DIR_PARAM NO_OUTPUT_DIRS)
endif (IS_MULTI_CONFIG)

#find_program(VIRTUALENV virtualenv)
#if(NOT VIRTUALENV)
#    message(FATAL_ERROR "Could not find `virtualenv` in PATH")
#endif()
#set(VIRTUALENV ${VIRTUALENV} -p python2.7)

conan_cmake_run(
    CONANFILE conan/conanfile.txt
    BASIC_SETUP
    CMAKE_TARGETS
    INSTALL_FOLDER ${CMAKE_BINARY_DIR}
    ${CUSTOM_NO_OUTPUT_DIR_PARAM}
    BUILD missing
)
