cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

SET(MYCOMPILE_FLAGS "-lzmq")
SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${MYCOMPILE_FLAGS}")

project(main LANGUAGES CXX)
add_executable(client ./src/client.cpp)
add_executable(server ./src/server.cpp)