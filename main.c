//
// Created by Cavide Balki Gemirter on 20.11.2020.
//

#include <stdio.h>
#include "common.c"

#define ADJACENT_COUNT 8

void assignValues(int row_count, int column_count, int matrix[row_count][column_count]);

void myAdjacents(int size, int row, int column, int matrix[size][size], int adjacents[ADJACENT_COUNT]);

int sumVector(int adjacents[ADJACENT_COUNT]);

int main(int argc, char *argv[]) {
    printf("Program started.\n");

    int N = 20;
    int epoch_count = 10;
    int matrix[N][N];

    assignValues(N, N, &matrix);

    printMatrix("Initial Matrix", N, N, matrix);

    for (int epoch = 0; epoch < epoch_count; epoch++) {
        printf("Epoch: %d\n", (epoch + 1));

        int new_matrix[N][N];
        for (int row = 0; row < N; row++) {
            for (int col = 0; col < N; col++) {
                int adjacents[ADJACENT_COUNT];
                myAdjacents(N, row, col, matrix, &adjacents);
                int sum = sumVector(adjacents);
                if (sum > 5) {
                    new_matrix[row][col] = 1;
                } else {
                    new_matrix[row][col] = 0;
                }
            }
        }

        printMatrix("Final Matrix", N, N, new_matrix);
    }
}

int sumVector(int adjacents[ADJACENT_COUNT]) {
    int sum = 0;
    for (int i = 0; i < ADJACENT_COUNT; i++) {
        sum += adjacents[i];
    }
    return sum;
}

void myAdjacents(int size, int row, int column, int matrix[size][size], int adjacents[ADJACENT_COUNT]) {
    int count = 0;
    for (int i = row - 1; i <= row + 1; i++) {
        if (i < 0) {
            continue;
        }
        for (int j = column - 1; j <= column + 1; j++) {
            if (j < 0) {
                continue;
            }
            if (i == row && j == column) {
                continue;
            }
            adjacents[count++] = matrix[i][j];
        }
    }
}

void assignValues(int row_count, int column_count, int matrix[row_count][column_count]) {
    for (int row = 0; row < row_count; row++) {
        for (int col = 0; col < column_count; col++) {
            matrix[row][col] = rand() % 2;
        }
    }
}