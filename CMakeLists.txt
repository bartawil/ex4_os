cmake_minimum_required(VERSION 3.16)
project(ex4_os C)

set(CMAKE_C_STANDARD 99)

add_executable(ex4_os ex4_srv.c ex4_client.c)
