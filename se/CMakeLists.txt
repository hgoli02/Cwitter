cmake_minimum_required(VERSION 3.17)
project(se C)

set(CMAKE_C_STANDARD 11)

add_executable(se main.c cJSON.c)

target_link_libraries(se ws2_32)