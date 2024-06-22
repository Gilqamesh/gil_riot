cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(project_name gil_riot_examples)

project(${project_name})

add_executable(test test.cpp)
target_link_libraries(test PRIVATE gilriot)

