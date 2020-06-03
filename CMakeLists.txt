cmake_minimum_required(VERSION 3.16)
project(radio_sk)

option(USE_CLANG "build with clang" ON)

set(CMAKE_CXX_STANDARD 17)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -std=c++17 -O2")
# TODO: give o2
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -std=c++17")

link_libraries("-lboost_program_options -lpthread")
include_directories(src)

add_executable(radio-proxy
        src/utils/types.h
        src/utils/sik_radio_protocol.h
        src/proxy/clients_handler_socket.h
        src/proxy/clients_pool.h
        src/proxy/audio_sender.h
        src/proxy/content_dispatcher.h
        src/proxy/proxy_server.h
        src/proxy/proxy.h src/utils/tcp.h src/utils/receiver.h src/utils/socket_wrapper.h src/utils/icy.h
        src/proxy/main.h
        src/utils/exceptions.h
        src/proxy.cc )
