#!/bin/bash
mpicc block.c -o block
mpirun -np 4 ./$1 $2

##example
# ./matrix_mul.sh block 4