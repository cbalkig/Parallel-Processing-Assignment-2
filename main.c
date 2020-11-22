//
// Created by Cavide Balki Gemirter on 20.11.2020.
//

#include <stdio.h>
#include <omp.h>
#include <time.h>
#include "common.c"

#define N               20
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

    printMatrix("Initial Matrix", N, N, matrix);

    for (int epoch = 0; epoch < EPOCH_COUNT; epoch++) {
        clock_t start = clock();

        int new_matrix[N][N];
        int i, j;
        #pragma omp parallel for num_threads(N * N) collapse(2) default(none) private(i, j) shared(verbose, matrix, new_matrix)
        for (i = 0; i < N; i++) {
            for (j = 0; j < N; j++) {
                if (verbose) {
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
        printMatrix("Final Matrix", N, N, new_matrix);

        memcpy(matrix, new_matrix, N * N * sizeof(int));
    }
}

int sumAdjacents(int size, int row, int column, int matrix[size][size]) {
    int sum = 0;
    int i, j;

    #pragma omp parallel for num_threads(9) collapse(2) default(none) private(i, j) shared(verbose, matrix, row, column) reduction (+:sum)
    for (i = row - 1; i <= row + 1; i++) {
        for (j = column - 1; j <= column + 1; j++) {
            if (verbose) {
                printf("sumAdjacents\ti = %d, j = %d, threadId = %d \n", i, j, omp_get_thread_num());
            }
            if (i < 0 || j < 0 || i >= N || j >= N || (i == row && j == column)) {
                sum += 0;
            }
            else {
                sum += matrix[i][j];
            }
        }
    }

    return sum;
}

void assignValues(int row_count, int column_count, int matrix[row_count][column_count]) {
    clock_t start = clock();

    int i, j;
    #pragma omp parallel for num_threads(N * N) collapse(2) default(none) private(i, j) shared(verbose, row_count, column_count, matrix)
    for (i = 0; i < row_count; i++) {
        for (j = 0; j < column_count; j++) {
            if (verbose) {
                printf("assignValues\ti = %d, j = %d, threadId = %d \n", i, j, omp_get_thread_num());
            }
            matrix[i][j] = rand() % 2;
        }
    }
    logTime("assignValues\t\t\t\t", start, clock());
}