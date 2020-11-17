//
// Created by Cavide Balki Gemirter on 16.11.2020.
//

#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    false, true
} bool;

int getMatrixARow(int i, int count, int size);

int getMatrixBCol(int i, int count, int size);

int getBlockSize(int N, int process_count);

int getParameters(int process_count, char **argv) {
    int size = atoi(argv[1]);
    if(size % process_count != 0){
        printf("Size and process count mismatch! %d - %d\n", size, process_count);
        exit(-1);
    }
    return size;
}

void readFile(char name[], int size, int x[size][size], int my_id) {
    FILE *file;

    file = fopen(name, "r");
    if (file == NULL) {
        printf("Process %d:\t\t\tFile read error: %s!\n", my_id, name);
        exit(-1);
    }

    int value;
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            fscanf(file, "%d*s", &value);
            x[i][j] = value;
        }
    }
    fclose(file);
}

void printMatrix(char *id, int rowCount, int columnCount, int matrix[rowCount][columnCount], int my_id) {
    char *s = (char *) malloc(1000 * sizeof(char));
    sprintf(s, "%s\n", id);
    for (int row = 0; row < rowCount; row++) {
        sprintf(s, "%s\t\t\t\t\t", s);
        for (int columns = 0; columns < columnCount; columns++) {
            sprintf(s, "%s%d\t", s, matrix[row][columns]);
        }
        sprintf(s, "%s\n", s);
    }
    printf("Process %d:\t\t\t%s", my_id, s);
    free(s);
}