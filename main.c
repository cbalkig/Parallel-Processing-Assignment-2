//#include <stdio.h>
//#include <mpi.h>
//#include <stdlib.h>
//#include <string.h>
//
//void main(){
//    int size = 4;
//    int process_count = 2;
//    int epochs = size * size * process_count;
//
//    for (int epoch = 1; epoch <= epochs; epoch++) {
//        int row2 = (epoch - 1) % size;
//        int col2 = ((epoch - 1) / (size / process_count)) % size;
//
//        printf("Epoch: %d\t[%d, %d]\n", epoch, row2, col2);
//    }
//}
//
////1   0   0     0   0
////2   1   0     2   0
////3   2   0     0   1
////4   3   0     2   1
////5   0   1     0   2
////6   1   1     2   2
////7   2   1
////8   3   1
////9   0   2