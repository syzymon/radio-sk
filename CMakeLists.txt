cmake_minimum_required(VERSION 3.16)
project(radio_sk)

option(USE_CLANG "build with clang" ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -std=c++17 -O2")

link_libraries("-lboost_program_options")
include_directories(src)

add_executable(radio_sk
        src/client.cc
        src/proxy.cc)
