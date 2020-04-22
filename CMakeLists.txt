cmake_minimum_required(VERSION 3.14)
project(huawei)

set(CMAKE_CXX_STANDARD 17)
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -O3 -pthread" )
add_executable(huawei main.cpp)