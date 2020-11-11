#!/bin/bash
mpicc block.c -o block
mpicc cyclic.c -o cyclic
mpirun -np $1 ./$3 $2

##example
# ./matrix_mul.sh 2 4 block
# ./matrix_mul.sh 2 4 cyclic