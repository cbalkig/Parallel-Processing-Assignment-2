//
// Created by Cavide Balki Gemirter on 20.11.2020.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    false, true
} bool;

int getParameters(int process_count, char **argv) {
    int size = atoi(argv[1]);
    if(size % process_count != 0){
        printf("Size and process count mismatch! %d - %d\n", size, process_count);
        exit(-1);
    }
    return size;
}

void printMatrix(int rowCount, int columnCount, int matrix[rowCount][columnCount]) {
    char *s = (char *) malloc(1000 * sizeof(char));
    for (int row = 0; row < rowCount; row++) {
        sprintf(s, "%s\t\t\t\t\t", s);
        for (int columns = 0; columns < columnCount; columns++) {
            sprintf(s, "%s%d\t", s, matrix[row][columns]);
        }
        sprintf(s, "%s\n", s);
    }
    printf(s);
    free(s);
}