//
// Created by Cavide Balki Gemirter on 20.11.2020.
//

#include <stdio.h>
#include <omp.h>
#include <time.h>
#include "common.c"

#define N               20
#define ADJACENT_COUNT  8
#define EPOCH_COUNT     10

void assignValues(int row_count, int column_count, int matrix[row_count][column_count]);

void myAdjacents(int size, int row, int column, int matrix[size][size], int adjacents[ADJACENT_COUNT]);

int sumVector(int adjacents[ADJACENT_COUNT]);

int main(int argc, char *argv[]) {
    printf("Program started.\n");

    int matrix[N][N];
    char *log = (char *) malloc(200 * sizeof(char));

    assignValues(N, N, &matrix);

    //printMatrix("Initial Matrix", N, N, matrix);

    for (int epoch = 0; epoch < EPOCH_COUNT; epoch++) {
        clock_t start = clock();

        int new_matrix[N][N];
        #pragma omp parallel for num_threads(N)
        for (int row = 0; row < N; row++) {
            #pragma omp parallel for num_threads(N)
            for (int col = 0; col < N; col++) {
                int adjacents[ADJACENT_COUNT];
                myAdjacents(N, row, col, matrix, &adjacents);
                if (sumVector(adjacents) > 5) {
                    new_matrix[row][col] = 1;
                } else {
                    new_matrix[row][col] = 0;
                }
            }
        }

        log = (char *) malloc(200 * sizeof(char));
        sprintf(log, "Epoch %d finished: \t\t\t", (epoch + 1));
        logTime(log, start, clock());
        //printMatrix("Final Matrix", N, N, new_matrix);
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

    #pragma omp parallel for num_threads(3)
    for (int i = row - 1; i <= row + 1; i++) {
        if (i < 0) {
            continue;
        }

        #pragma omp parallel for num_threads(3)
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
    clock_t start = clock();
    #pragma omp parallel for collapse(2) num_threads(N)
    for (int row = 0; row < row_count; row++) {
        for (int col = 0; col < column_count; col++) {
            matrix[row][col] = rand() % 2;
        }
    }
    logTime("assignValues\t\t\t\t", start, clock());
}