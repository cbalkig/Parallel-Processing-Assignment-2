//
// Created by Cavide Balki Gemirter on 16.11.2020.
//

#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <string.h>
#include "common.c"

int getMatrixARow(int i, int count, int size);

int getMatrixBCol(int i, int count, int size);

int main(int argc, char *argv[]) {
    // Some logging
    printf("Block Data Structure - Program started.\n");

    // Declarations
    bool verbose = true;
    char *matrixA_file_name = (char *) malloc(1000 * sizeof(char));
    char *matrixB_file_name = (char *) malloc(1000 * sizeof(char));
    char *log = (char *) malloc(200 * sizeof(char));
    int my_name_len, process_count, my_id, err, row, col;
    char my_name[MPI_MAX_PROCESSOR_NAME];
    MPI_Status status;
    int root = 0;

    // MPI Init
    err = MPI_Init(&argc, &argv);
    if (err != 0) {
        printf("Init Error %d.", err);
        exit(-1);
    }

    MPI_Comm_size(MPI_COMM_WORLD, &process_count);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_id);
    MPI_Get_processor_name(my_name, &my_name_len);
    printf("Process %d:\t\t\tProcessor Count: %d\t\tMy ID: %d\t\tMy Name: %s\n", my_id, process_count, my_id,
           my_name);

    int N = getParameters(process_count, argv);
    sprintf(matrixA_file_name, "%s%d%s", "/Users/balki/CLionProjects/Assignment-1/files/", N, "/matrixA.txt");
    sprintf(matrixB_file_name, "%s%d%s", "/Users/balki/CLionProjects/Assignment-1/files/", N, "/matrixB.txt");

    // Detect and send matrix size, slider.
    err = MPI_Bcast(&N, 1, MPI_INT, root, MPI_COMM_WORLD);
    if (err != 0) {
        printf("Process %d:\t\t\t!!ERROR: Broadcast N to workers: %d.\n", my_id, err);
        exit(-1);
    }

    int half_process_count = process_count / 2;
    int block_size = N / half_process_count;
    if (my_id == root){
        printf("Process %d:\t\t\tN: %d\tBlock size: %d\n", my_id, N, block_size);
    }

    // MPI Custom Data Types
    MPI_Datatype matrixB_type;
    MPI_Type_vector(N, block_size, N, MPI_INT, &matrixB_type);
    MPI_Type_commit(&matrixB_type);

    MPI_Datatype matrixC_type;
    MPI_Type_vector(block_size, block_size, N, MPI_INT, &matrixC_type);
    MPI_Type_commit(&matrixC_type);

    // Read matrix
    int matrixA[N][N], matrixB[N][N], matrixC[N][N];
    if (my_id == root) {
        readFile(matrixA_file_name, N, &matrixA, my_id);
        printMatrix("Original Matrix A is", N, N, matrixA, my_id);

        readFile(matrixB_file_name, N, &matrixB, my_id);
        printMatrix("Original Matrix B is", N, N, matrixB, my_id);

        if (verbose) printf("Process %d:\t\t\tThe files are closed.\n", my_id);
    }

    // Split and then send & receive vectors
    int my_matrixA[block_size][N], my_matrixB[N][block_size];
    if (my_id == root) {
        for (int i = 1; i < process_count; i++) {
            row = getMatrixARow(i, process_count, block_size);
            err = MPI_Send(&matrixA[row], block_size * N, MPI_INT, i, 0, MPI_COMM_WORLD);
            if (err != 0) {
                printf("Process %d:\t\t\t!!ERROR: Send Matrix A to workers: %d.\n", my_id, err);
                exit(-1);
            }
        }

        row = getMatrixARow(my_id, process_count, block_size);
        err = MPI_Sendrecv(&matrixA[row], block_size * N, MPI_INT, root, 0, &my_matrixA, block_size * N, MPI_INT, root, 0,
                           MPI_COMM_WORLD, &status);
        if (err != 0) {
            printf("Process %d:\t\t\t!!ERROR: Send Matrix A to Master: %d.\n", my_id, err);
            exit(-1);
        }
    } else {
        err = MPI_Recv(&my_matrixA, block_size * N, MPI_INT, root, 0, MPI_COMM_WORLD, &status);
        if (err != 0) {
            printf("Process %d:\t\t\t!!ERROR: Received Matrix A from Master: %d.\n", my_id, err);
            exit(-1);
        }
    }
    if (verbose) {
        printMatrix("My Matrix A", block_size, N, my_matrixA, my_id);
    }

    if (my_id == root) {
        for (int i = 1; i < process_count; i++) {
            col = getMatrixBCol(i, process_count, block_size);
            err = MPI_Send(&matrixB[0][col], 1, matrixB_type, i, 0, MPI_COMM_WORLD);
            if (err != 0) {
                printf("Process %d:\t\t\t!!ERROR: Send Matrix B to workers: %d.\n", my_id, err);
                exit(-1);
            }
        }

        col = getMatrixBCol(my_id, process_count, block_size);
        err = MPI_Sendrecv(&matrixB[0][col], 1, matrixB_type, root, 0, &my_matrixB, block_size * N, MPI_INT, root, 0,
                           MPI_COMM_WORLD, &status);
        if (err != 0) {
            printf("Process %d:\t\t\t!!ERROR: Send Matrix B to Master: %d.\n", my_id, err);
            exit(-1);
        }
    } else {
        err = MPI_Recv(&my_matrixB, block_size * N, MPI_INT, root, 0, MPI_COMM_WORLD, &status);
        if (err != 0) {
            printf("Process %d:\t\t\t!!ERROR: Received Matrix B from Master: %d.\n", my_id, err);
            exit(-1);
        }
    }
    if (verbose) {
        printMatrix("My Matrix B", N, block_size, my_matrixB, my_id);
    }

    // Do the calculations
    if (verbose) printf("Process %d:\t\t\tStarted calculations.\n", my_id);
    int my_result[block_size][block_size];
    for (int i = 0; i < block_size; i++) {
        for (int j = 0; j < block_size; j++) {
            my_result[i][j] = 0;
            for (int k = 0; k < N; k++) {
                my_result[i][j] += (my_matrixA[i][k] * my_matrixB[k][j]);
            }
        }
    }

    row = getMatrixARow(my_id, process_count, block_size);
    col = getMatrixBCol(my_id, process_count, block_size);
    log = (char *) malloc(200 * sizeof(char));
    sprintf(log, "My Result (Row: %d\tCol: %d)", row, col);
    printMatrix(log, block_size, block_size, my_result, my_id);

    // Send or get the calculations.
    if (my_id == root) {
        if (process_count > 1) {
            for(int i=1; i<process_count; i++){
                row = getMatrixARow(i, process_count, block_size);
                col = getMatrixBCol(i, process_count, block_size);
                err = MPI_Recv(&matrixC[row][col], 1, matrixC_type, i, 0, MPI_COMM_WORLD, &status);
                if (err != 0) {
                    printf("Process %d:\t\t\t!!ERROR: Received result from worker: %d.\n", i, err);
                    exit(-1);
                }
            }
        }

        row = getMatrixARow(my_id, process_count, block_size);
        col = getMatrixBCol(my_id, process_count, block_size);
        err = MPI_Sendrecv(&my_result, block_size * block_size, MPI_INT, root, 0, &matrixC[row][col], 1, matrixC_type, root, 0,
                           MPI_COMM_WORLD, &status);
        if (err != 0) {
            printf("Process %d:\t\t\t!!ERROR: Send result to Master: %d.\n", my_id, err);
            exit(-1);
        }
    } else {
        err = MPI_Send(&my_result, block_size * block_size, MPI_INT, root, 0, MPI_COMM_WORLD);
        if (err != 0) {
            printf("Process %d:\t\t\t!!ERROR: Send result to Master: %d.\n", my_id, err);
            exit(-1);
        }
    }

    if(my_id == root) {
        printMatrix("Matrix C is", N, N, matrixC, my_id);
    }

    MPI_Finalize();

    free(log);
    free(matrixA_file_name);
    free(matrixB_file_name);
}

int getMatrixARow(int i, int process_count, int block_size) {
    return (i / (process_count / 2)) * block_size;
}

int getMatrixBCol(int i, int process_count, int block_size) {
    return (i % (process_count / 2)) * block_size;
}