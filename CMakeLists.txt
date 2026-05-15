cmake_minimum_required(VERSION 3.10)

project(go)

set(CMAKE_CXX_STANDARD 11)

include_directories(src)

add_executable(go src/msh.h src/msh.cpp src/main.cpp)