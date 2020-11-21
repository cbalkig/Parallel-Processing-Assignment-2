//
// Created by Cavide Balki Gemirter on 20.11.2020.
//

#include <stdio.h>
#include "common.c"

void assignValues(int rowCount, int columnCount, int matrix[rowCount][columnCount]);

int main(int argc, char *argv[]) {
    printf("Program started.\n");

    int N = 20;
    int matrix[N][N];

    assignValues(N, N, &matrix);

    printMatrix(N, N, matrix);
}

void assignValues(int rowCount, int columnCount, int matrix[rowCount][columnCount]) {
    for (int row = 0; row < rowCount; row++) {
        for (int columns = 0; columns < columnCount; columns++) {
            matrix[row][columns] = rand() % 2;
        }
    }
}