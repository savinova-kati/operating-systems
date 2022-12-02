cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

SET(MYCOMPILE_FLAGS "-pthread")
SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${MYCOMPILE_FLAGS}")

project(main LANGUAGES CXX)
add_executable(main ./src/lab.cpp)