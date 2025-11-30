cmake_minimum_required(VERSION 3.10)

include(FetchContent)
FetchContent_Declare(
    gilweb
    GIT_REPOSITORY https://github.com/your/gil_web.git
    GIT_TAG        main
)
FetchContent_MakeAvailable(gilweb)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(lib_name gilriot)

project(${lib_name} VERSION 0.1)
add_subdirectory(example)
add_library(${lib_name} STATIC riot.cpp)
configure_file(riot.h.in riot.h)
target_link_libraries(${lib_name} PUBLIC gilweb)
target_include_directories(${lib_name} PUBLIC "${PROJECT_BINARY_DIR}" "${PROJECT_SOURCE_DIR}")

