cmake_minimum_required(VERSION 3.10)

project(go)

set(CMAKE_CXX_STANDARD 11)

include_directories(src)

# Exécutable principal (avec predicates.c)
add_executable(go src/msh.h src/msh.cpp src/main.cpp src/predicates.c)

# Exécutable de tests
add_executable(test_basic tests/test_basic.cpp src/msh.cpp src/msh.h src/predicates.c)