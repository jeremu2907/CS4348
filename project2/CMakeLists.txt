cmake_minimum_required(VERSION 3.25)
project(proj2)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lpthread -pthread")

set(
    MY_SOURCES

    main.cpp
    utils.h
)

find_package(Threads REQUIRED)

add_executable(proj_2 ${MY_SOURCES})

target_link_libraries(proj_2 PRIVATE Threads::Threads)