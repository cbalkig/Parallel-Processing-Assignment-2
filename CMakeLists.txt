cmake_minimum_required(VERSION 3.15)
project(Assignment-2 C)

set(CMAKE_C_STANDARD 99)

find_package(OpenMP REQUIRED)
add_executable(Assignment-2 parallel_v1.c)
target_link_libraries(Assignment-2 PRIVATE OpenMP::OpenMP_C)