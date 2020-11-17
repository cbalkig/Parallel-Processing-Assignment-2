#!/bin/bash
mpicc block.c -o block
mpicc cyclic.c -o cyclic
mpirun -np 4 ./$1 $2

##example
# ./matrix_mul.sh block 4
# ./matrix_mul.sh cyclic 4