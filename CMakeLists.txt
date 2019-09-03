cmake_minimum_required(VERSION 3.10)
project(ProgettoApi2019 C)

set(CMAKE_C_STANDARD 11)

add_executable(ProgettoApi2019 main.c)

target_link_libraries(ProgettoApi2019 m)
SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -lm -std=11")
