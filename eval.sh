#!/bin/bash
gcc-10 -o playGame -fopenmp parallel_v5.c

for i in 1 2 3 4 5 6 7 8 9 10
do
  ./playGame
done