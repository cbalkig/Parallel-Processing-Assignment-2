#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <string.h>

void main(){
    int size = 4;
    int process_count = 1;
    int epochs = size * size * process_count;

    for (int epoch = 1; epoch <= epochs; epoch++) {
        int row2 = ((epoch - 1) % (size / process_count)) * (size / process_count);
        int col2 = ((epoch - 1) / (size / process_count)) % size;

        printf("Epoch: %d\t[%d, %d]\n", epoch, row2, col2);
    }
}

////1   0   0     0   0     0   0
////2   1   0     2   0     0   1
////3   2   0     0   1     0   2
////4   3   0     2   1     0   3
////5   0   1     0   2     0   0
////6   1   1     2   2     0   1
////7   2   1     0   3     0   2
////8   3   1     2   3     0   3
////9   0   2     0   0     0   0