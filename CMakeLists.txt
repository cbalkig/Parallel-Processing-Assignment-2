cmake_minimum_required(VERSION 3.15)
project(Assignment-1 C)

set(CMAKE_C_STANDARD 99)

add_executable(Assignment-1 block.c cyclic.c main.c)
set(CMAKE_C_COMPILER mpicc)
set(CMAKE_CXX_COMPILER mpicxx)
target_link_libraries(Assignment-1 mpi)