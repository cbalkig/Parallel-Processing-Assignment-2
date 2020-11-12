#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    false, true
} bool;

int getLineCount(char name[], int my_id);

void readFile(char name[], int size, int x[size][size], int my_id);

void printMatrix(char *id, int rowCount, int columnCount, int matrix[rowCount][columnCount], int my_id);

void printVector(char *id, int size, int vector[size], int my_id);

int main(int argc, char *argv[]) {
    // Some logging
    printf("Cyclic Data Structure - Program started.\n");

    int size = atoi(argv[1]);

    // Declarations
    bool verbose = false;
    char *matrix1_file_name = (char *) malloc(1000 * sizeof(char));
    char *matrix2_file_name = (char *) malloc(1000 * sizeof(char));
    char *log = (char *) malloc(200 * sizeof(char));
    sprintf(matrix1_file_name, "%s%d%s", "/Users/balki/CLionProjects/Assignment-1/files/", size, "/matrix1.txt");
    sprintf(matrix2_file_name, "%s%d%s", "/Users/balki/CLionProjects/Assignment-1/files/", size, "/matrix2.txt");
    int epochs, my_name_len, process_count, my_id, err;
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
    printf("Process %d:\t\t\tInputs:\t\tProcessor Count: %d\t\tMy ID: %d\t\tMy Name: %s\n", my_id, process_count, my_id,
           my_name);

    // Detect and send matrix size, slider.
    if (my_id == root) {
        size = getLineCount(matrix1_file_name, my_id);
    }
    err = MPI_Bcast(&size, 1, MPI_INT, root, MPI_COMM_WORLD);
    if (err != 0) {
        printf("Process %d:\t\t\t!!ERROR: Broadcast size to workers: %d.\n", my_id, err);
        exit(-1);
    }
    epochs = (size * size * size / process_count);
    printf("Process %d:\t\t\tMy size: %d\tEpochs: %d.\n", my_id, size, epochs);

    // MPI Custom Data Types
    MPI_Datatype matrix2_type;
    MPI_Type_vector(size, 1, size, MPI_INT, &matrix2_type);
    MPI_Type_commit(&matrix2_type);

    // Read matrix
    int matrix1[size][size], matrix2[size][size], final[size][size];
    for (int i=0; i<size; i++){
        for (int j=0; j<size; j++){
            final[i][j] = 0;
        }
    }

    if (my_id == root) {
        readFile(matrix1_file_name, size, &matrix1, my_id);
        printMatrix("Full Matrix 1 is", size, size, matrix1, my_id);

        readFile(matrix2_file_name, size, &matrix2, my_id);
        printMatrix("Full Matrix 2 is", size, size, matrix2, my_id);

        if (verbose) printf("Process %d:\t\t\tThe files are closed.\n", my_id);
    }

    //Run epochs
    for (int epoch = 1; epoch <= epochs; epoch++) {
        // Split and then send & receive vectors
        int my_value1;
        int my_vector2[size];
        int row = (epoch - 1) / ((size / process_count) * size);
        int col = ((epoch - 1) * process_count) % size;
        if (my_id == root) {
            for (int i = 1; i < process_count; i++) {
                err = MPI_Send(&matrix1[row][col + i], 1, MPI_INT, i, 0, MPI_COMM_WORLD);
                if (err != 0) {
                    printf("Process %d:\t\t\t!!ERROR: Send Vector 1 to workers: %d.\n", my_id, err);
                    exit(-1);
                }
            }

            err = MPI_Sendrecv(&matrix1[row][col], 1, MPI_INT, root, 0, &my_value1, 1, MPI_INT, root, 0,
                               MPI_COMM_WORLD, &status);
            if (err != 0) {
                printf("Process %d:\t\t\t!!ERROR: Send Value 1 to Master: %d.\n", my_id, err);
                exit(-1);
            }
        } else {
            err = MPI_Recv(&my_value1, 1, MPI_INT, root, 0, MPI_COMM_WORLD, &status);
            if (err != 0) {
                printf("Process %d:\t\t\t!!ERROR: Received Value 1 from Master: %d.\n", my_id, err);
                exit(-1);
            }
        }
        if (verbose) {
            printf("Process %d:\t\t\tMy Value 1: %d (Epoch: %d)\n", my_id, my_value1, epoch);
        }

        row = ((epoch - 1) % (size * (size / process_count))) / (size / process_count);
        if (my_id == root) {
            for (int i = 1; i < process_count; i++) {
                err = MPI_Send(&matrix2[0][row], 1, matrix2_type, i, 0, MPI_COMM_WORLD);
                if (err != 0) {
                    printf("Process %d:\t\t\t!!ERROR: Send Vector 2 to workers: %d.\n", my_id, err);
                    exit(-1);
                }
            }

            err = MPI_Sendrecv(&matrix2[0][row], 1, matrix2_type, root, 0, &my_vector2, size, MPI_INT, root, 0,
                               MPI_COMM_WORLD, &status);
            if (err != 0) {
                printf("Process %d:\t\t\t!!ERROR: Send Vector 2 to Master: %d.\n", my_id, err);
                exit(-1);
            }
        } else {
            err = MPI_Recv(&my_vector2, size, MPI_INT, root, 0, MPI_COMM_WORLD, &status);
            if (err != 0) {
                printf("Process %d:\t\t\t!!ERROR: Received Vector 2 from Master: %d.\n", my_id, err);
                exit(-1);
            }
        }
        if (verbose) {
            log = (char *) malloc(200 * sizeof(char));
            sprintf(log, "My Vector 2 (Epoch: %d\tRow: %d)", epoch, row);
            printVector(log, size, my_vector2, my_id);
        }

        // Do the calculations
        if (verbose) printf("Process %d:\t\t\tStarted calculations.\n", my_id);
        int my_result = 0;
        for (int i = 0; i < size; i++) {
            my_result += (my_value1 * my_vector2[i]);
        }

        // Send or get the calculations.
        if (my_id == root) {
            int row = (epoch - 1) / ((size / process_count) * size);
            int col = ((epoch - 1) % (size * (size / process_count))) / (size / process_count);

            if (process_count > 1) {
                for(int i=1; i<process_count; i++){
                    err = MPI_Recv(&final[row][col], 1, MPI_INT, i, 0, MPI_COMM_WORLD, &status);
                    if (err != 0) {
                        printf("Process %d:\t\t\t!!ERROR: Received result from worker: %d.\n", i, err);
                        exit(-1);
                    }
                    printMatrix("Final Matrix is", size, size, final, my_id);
                }
            }

            err = MPI_Sendrecv(&my_result, 1, MPI_INT, root, 0, &final[row][col], 1, MPI_INT, root, 0,
                               MPI_COMM_WORLD, &status);
            if (err != 0) {
                printf("Process %d:\t\t\t!!ERROR: Send result to Master: %d.\n", my_id, err);
                exit(-1);
            }
            printMatrix("Final Matrix is", size, size, final, my_id);
        } else {
            err = MPI_Send(&my_result, 1, MPI_INT, root, 0, MPI_COMM_WORLD);
            if (err != 0) {
                printf("Process %d:\t\t\t!!ERROR: Send result to Master: %d.\n", my_id, err);
                exit(-1);
            }
        }
    }

    if(my_id == root) {
        printMatrix("Final Matrix is", size, size, final, my_id);
    }

    MPI_Finalize();

    free(log);
    free(matrix1_file_name);
    free(matrix2_file_name);
}

int getLineCount(char name[], int my_id) {
    FILE *file;

    file = fopen(name, "r");
    if (file == NULL) {
        printf("Process %d:\t\t\tFile read error: %s!\n", my_id, name);
        exit(-1);
    }

    int count_lines = 0;
    char chr = chr = getc(file);
    while (chr != EOF) {
        if (chr == '\n') {
            count_lines++;
        }
        chr = getc(file);
    }
    fclose(file);
    return count_lines + 1;
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
            sprintf(s, "%s%d\t\t", s, matrix[row][columns]);
        }
        sprintf(s, "%s\n", s);
    }
    printf("Process %d:\t\t\t%s", my_id, s);
    free(s);
}

void printVector(char *id, int size, int vector[size], int my_id) {
    char *s = (char *) malloc(1000 * sizeof(char));
    sprintf(s, "%s\n", id);
    sprintf(s, "%s\t\t\t\t\t", s);
    for (int row = 0; row < size; row++) {
        sprintf(s, "%s%d\t\t", s, vector[row]);
    }
    printf("Process %d:\t\t\t%s\n", my_id, s);
    free(s);
}