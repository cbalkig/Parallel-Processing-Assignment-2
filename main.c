//
// Created by Cavide Balki Gemirter on 20.11.2020.
//

#include <stdio.h>
#include <omp.h>
#include <time.h>
#include "common.c"

#define N               3
#define EPOCH_COUNT     10

bool verbose = false;

void assignValues(int row_count, int column_count, int matrix[row_count][column_count]);

int sumAdjacents(int size, int row, int column, int matrix[size][size]);

int main(int argc, char *argv[]) {
    printf("Program started.\n");

    //Declarations
    int matrix[N][N];
    char *log = (char *) malloc(200 * sizeof(char));

    assignValues(N, N, &matrix);

    //printMatrix("Initial Matrix", N, N, matrix);

    for (int epoch = 0; epoch < EPOCH_COUNT; epoch++) {
        clock_t start = clock();

        int new_matrix[N][N];
        #pragma omp parallel for num_threads(N * N) collapse(2)
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                if(verbose) {
                    printf("fullOperation\ti = %d, j = %d, threadId = %d \n", i, j, omp_get_thread_num());
                }
                int sum = sumAdjacents(N, i, j, matrix);
                if (sum > 5) {
                    new_matrix[i][j] = 1;
                } else {
                    new_matrix[i][j] = 0;
                }
            }
        }

        log = (char *) malloc(200 * sizeof(char));
        sprintf(log, "Epoch %d finished: \t\t\t", (epoch + 1));
        logTime(log, start, clock());
        //printMatrix("Final Matrix", N, N, new_matrix);
    }
}

int sumAdjacents(int size, int row, int column, int matrix[size][size]) {
    int sum = 0;

    #pragma omp parallel for num_threads(9) collapse(2) reduction (+:sum)
    for (int i = row - 1; i <= row + 1; i++) {
        for (int j = column - 1; j <= column + 1; j++) {
            if(verbose) {
                printf("sumAdjacents\ti = %d, j = %d, threadId = %d \n", i, j, omp_get_thread_num());
            }

            if (i < 0) {
                continue;
            }
            if (j < 0) {
                continue;
            }
            if (i == row && j == column) {
                continue;
            }
            sum += matrix[i][j];
        }
    }

    return sum;
}

void assignValues(int row_count, int column_count, int matrix[row_count][column_count]) {
    clock_t start = clock();
    #pragma omp parallel for num_threads(N * N) collapse(2)
    for (int row = 0; row < row_count; row++) {
        for (int col = 0; col < column_count; col++) {
            if(verbose) {
                printf("assignValues\ti = %d, j = %d, threadId = %d \n", row, col, omp_get_thread_num());
            }
            matrix[row][col] = rand() % 2;
        }
    }
    logTime("assignValues\t\t\t\t", start, clock());
}