cmake_minimum_required(VERSION 3.8)
project(simplehttpserver)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        http-parser-test.c
        http-parser.c
        http-parser.h
        Makefile
        simple-http-server.c
        single-thread-http-server.c)

add_executable(simplehttpserver ${SOURCE_FILES})