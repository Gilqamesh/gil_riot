cmake_minimum_required(VERSION 3.10)

# Initialize + update git submodules recursively
find_package(Git QUIET)
if(GIT_FOUND AND EXISTS "${CMAKE_SOURCE_DIR}/.git")
    message(STATUS "Updating git submodules (recursive)…")
    execute_process(
        COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        RESULT_VARIABLE GIT_SUBMOD_RESULT
    )

    if(NOT GIT_SUBMOD_RESULT EQUAL 0)
        message(FATAL_ERROR "git submodule update --init --recursive failed")
    endif()
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(lib_name gilriot)

project(${lib_name} VERSION 0.1)
add_subdirectory(gil_web)
add_subdirectory(example)
add_library(${lib_name} STATIC riot.cpp)
configure_file(riot.h.in riot.h)
target_link_libraries(${lib_name} PUBLIC gilweb)
target_include_directories(${lib_name} PUBLIC "${PROJECT_BINARY_DIR}" "${PROJECT_SOURCE_DIR}")

