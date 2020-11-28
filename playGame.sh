#!/bin/bash
gcc-10 -o playGame -fopenmp main.c
#export OMP_NUM_THREADS=4
#launchctl setenv OMP_NUM_THREADS=4
./playGame