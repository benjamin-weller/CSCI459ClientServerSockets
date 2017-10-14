cmake_minimum_required(VERSION 3.8)
project(CSCI459Server)

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES main.c server.c server.h client.c client.h)
add_executable(CSCI459Server ${SOURCE_FILES})