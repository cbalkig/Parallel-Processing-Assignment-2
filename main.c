//
// Created by Cavide Balki Gemirter on 20.11.2020.
//

#include <stdio.h>
#include <omp.h>
#include <time.h>
#include "common.c"

#define N               200
#define EPOCH_COUNT     10
#define MAX_THREADS     100

bool verbose = false;
bool print_results = false;

void assignValues(int matrix[N][N]);

int sumAdjacents(int size, int row, int column, int matrix[size][size]);

int main(int argc, char *argv[]) {
    printf("Program started.\n");
    clock_t start = clock();

    omp_set_max_active_levels(3);

    //Declarations
    int matrix[N][N];
    char *log = (char *) malloc(200 * sizeof(char));

    assignValues(&matrix);

    if (print_results) {
        printMatrix("Initial Matrix", N, N, matrix);
    }

    for (int epoch = 0; epoch < EPOCH_COUNT; epoch++) {
        clock_t start = clock();

        int new_matrix[N][N];
        int i, j;
#pragma omp parallel for schedule(static, 1) num_threads(MAX_THREADS) collapse(2) default(none) private(i, j) shared(verbose, matrix, new_matrix)
        for (i = 0; i < N; i++) {
            for (j = 0; j < N; j++) {
                if (verbose) {
                    printf("fullOperation\ti = %d, j = %d, threadId = %d \n", i, j, omp_get_thread_num());
                }
                if (sumAdjacents(N, i, j, matrix) > 5) {
                    new_matrix[i][j] = 1;
                } else {
                    new_matrix[i][j] = 0;
                }
            }
        }

        log = (char *) malloc(200 * sizeof(char));
        sprintf(log, "Epoch %d finished: \t\t\t", (epoch + 1));
        logTime(log, start, clock());
        if (print_results) {
            printMatrix("Final Matrix", N, N, new_matrix);
        }

        memcpy(matrix, new_matrix, MAX_THREADS * sizeof(int));
    }

    logTime("Program finished: \t\t\t", start, clock());

    free(log);
}

int sumAdjacents(int size, int row, int column, int matrix[size][size]) {
    int sum = 0;

    int min_row = row - 1;
    int min_column = column - 1;
    int max_row = row + 1;
    int max_column = column + 1;

    int i, j;
//#pragma omp parallel for schedule(static, 1) num_threads(9) collapse(2) default(none) private(i, j) shared(verbose, matrix, min_row, min_column, max_row, max_column) reduction (+:sum)
    for (i = min_row; i <= max_row; i++) {
        for (j = min_column; j <= max_column; j++) {
            if (verbose) {
                printf("sumAdjacents\ti = %d, j = %d, threadId = %d \n", i, j, omp_get_thread_num());
            }
            if (i < 0 || j < 0 || i >= N || j >= N) {
                sum += 0;
            } else {
                sum += matrix[i][j];
            }
        }
    }

    return sum;
}

void assignValues(int matrix[N][N]) {
    clock_t start = clock();

    int i, j;
#pragma omp parallel for schedule(static, 1) num_threads(MAX_THREADS) collapse(2) default(none) private(i, j) shared(verbose, matrix)
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            if (verbose) {
                printf("assignValues\ti = %d, j = %d, threadId = %d \n", i, j, omp_get_thread_num());
            }
            matrix[i][j] = rand() % 2;
        }
    }
    logTime("assignValues\t\t\t\t", start, clock());
}