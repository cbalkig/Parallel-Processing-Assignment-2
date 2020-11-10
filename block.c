#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <string.h>

int getLineCount(char name[], int my_id);

void readFile(char name[], int size, int x[size][size], int my_id);

void printMatrix(char *id, int size, int matrix[size][size], int my_id);

void printVector(char *id, int size, int vector[size], int my_id);

int main(int argc, char *argv[]) {
    // Some logging
    printf("Block Data Structure - Program started.\n");

    // Declarations
    char matrix1_file_name[] = "/Users/balki/CLionProjects/Assignment-1/matrix1.txt";
    char matrix2_file_name[] = "/Users/balki/CLionProjects/Assignment-1/matrix2.txt";
    int size, slider, block_size, epochs, my_name_len, process_count, my_id, err, result;
    char my_name[MPI_MAX_PROCESSOR_NAME];
    MPI_Status status;
    int root = 0;

    // MPI Init
    err = MPI_Init(&argc, &argv);
    if (err == 0) {
        printf("Init OK.\n");
    } else {
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
        printf("Process %d:\t\t\tSize=%d\n", my_id, size);
        printf("Process %d:\t\t\tSlider=%d\n", my_id, slider);
    }

    err = MPI_Bcast(&size, 1, MPI_INT, root, MPI_COMM_WORLD);
    if (err != 0) {
        printf("Process %d:\t\t\t!!ERROR: Broadcast size to workers: %d.\n", my_id, err);
        exit(-1);
    }
    slider = (size * size) / process_count;
    block_size = (slider / size);
    epochs = block_size;
    printf("Process %d:\t\t\tMy size: %d\tSlider: %d\tBlock size: %d\tEpochs: %d.\n", my_id, size, slider, block_size, epochs);

    // Read matrix
    int matrix1[size][size], matrix2[size][size], final[size][size];
    if (my_id == root) {
        readFile(matrix1_file_name, size, &matrix1, my_id);
        printMatrix("Full Matrix 1 is", size, matrix1, my_id);

        readFile(matrix2_file_name, size, &matrix2, my_id);
        printMatrix("Full Matrix 2 is", size, matrix2, my_id);

        printf("Process %d:\t\t\tThe files are closed.\n", my_id);
    }

    //Run epochs
    for (int epoch = 1; epoch <= epochs; epoch++) {
        printf("Process %d:\t\t\tEpoch: %d.\n", my_id, epoch);

        // Split and then send & receive vectors
        int my_matrix1[slider], my_matrix2[slider];
        int row = ((epoch - 1) / block_size) * block_size;

        if(my_id == root){
            for (int i=1; i<process_count; i++){
                err = MPI_Send(&matrix1[row], slider, MPI_INT, i, 0, MPI_COMM_WORLD);
                if (err != 0) {
                    printf("Process %d:\t\t\t!!ERROR: Send Vector 1 to workers: %d.\n", my_id, err);
                    exit(-1);
                }
            }

            err = MPI_Sendrecv(&matrix1[row], slider, MPI_INT, root, 0, &my_matrix1, slider, MPI_INT, root, 0,
                               MPI_COMM_WORLD, &status);
            if (err != 0) {
                printf("Process %d:\t\t\t!!ERROR: Send Vector 1 to Master: %d.\n", my_id, err);
                exit(-1);
            }
        }
        else{
            err = MPI_Recv(&my_matrix1, slider, MPI_INT, root, 0, MPI_COMM_WORLD, &status);
            if (err != 0) {
                printf("Process %d:\t\t\t!!ERROR: Received Vector 1 from Master: %d.\n", my_id, err);
                exit(-1);
            }
        }
        printVector("Vector 1", slider, my_matrix1, my_id);

        MPI_Datatype matrix2_type;
        MPI_Type_vector(size, block_size, size, MPI_INT, &matrix2_type);
        MPI_Type_commit(&matrix2_type);

        for (int count = 0; count < size; count++) {
            if (my_id == root) {
                for (int i = 1; i < process_count; i++) {
                    err = MPI_Send(&matrix2[0][(i * block_size)], 1, matrix2_type, i, 0, MPI_COMM_WORLD);
                    if (err == 0) {
                        printf("Process %d:\t\t\tSend Vector 2 to workers.\n", my_id);
                    } else {
                        printf("Process %d:\t\t\t!!ERROR: Send Vector 2 to workers: %d.\n", my_id, err);
                        exit(-1);
                    }
                }

                err = MPI_Sendrecv(&matrix2, 1, matrix2_type, root, 0, &my_matrix2, slider, MPI_INT, root, 0,
                                   MPI_COMM_WORLD, &status);
                if (err == 0) {
                    printf("Process %d:\t\t\tSend Vector 2 to Master.\n", my_id);
                } else {
                    printf("Process %d:\t\t\t!!ERROR: Send Vector 2 to Master: %d.\n", my_id, err);
                    exit(-1);
                }
            } else {
                err = MPI_Recv(&my_matrix2, slider, MPI_INT, root, 0, MPI_COMM_WORLD, &status);
                if (err == 0) {
                    printf("Process %d:\t\t\tReceived Vector 2 to workers.\n", my_id);
                } else {
                    printf("Process %d:\t\t\t!!ERROR: Received Vector 2 from Master: %d.\n", my_id, err);
                    exit(-1);
                }
            }
            printVector("Vector 2", slider, my_matrix2, my_id);

            // Do the calculations
            /*printf("Process %d:\t\t\tStarted calculations.\n", my_id);
            result = 0;
            for (int i = 0; i < slider; i++) {
                result += (my_matrix1[i] * my_matrix2[i]);
            }
            printf("Process %d:\t\t\tCalculations finished, My Result=%d\n", my_id, result);

            // Send or get the calculations.
            err = MPI_Gather(&result, 1, MPI_INT, &final, 1, MPI_INT, root, MPI_COMM_WORLD);
            if (err == 0) {
                printf("Process %d:\t\t\tGather result to master.\n", my_id);
            } else {
                printf("Process %d:\t\t\t!!ERROR: Gather result to master: %d.\n", my_id, err);
                exit(-1);
            }

            printMatrix("Final Matrix is", size, final, my_id);*/
        }
    }

    MPI_Finalize();
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

void printMatrix(char *id, int size, int matrix[size][size], int my_id) {
    char *s = (char *) malloc(100 * sizeof(char));
    sprintf(s, "%s\n", id);
    for (int row = 0; row < size; row++) {
        sprintf(s, "%s\t\t\t\t\t", s);
        for (int columns = 0; columns < size; columns++) {
            sprintf(s, "%s%d\t\t", s, matrix[row][columns]);
        }
        sprintf(s, "%s\n", s);
    }
    printf("Process %d:\t\t\t%s", my_id, s);
}

void printVector(char *id, int size, int vector[size], int my_id) {
    char *s = (char *) malloc(100 * sizeof(char));
    sprintf(s, "%s\n", id);
    sprintf(s, "%s\t\t\t\t\t", s);
    for (int row = 0; row < size; row++) {
        sprintf(s, "%s%d\t\t", s, vector[row]);
    }
    printf("Process %d:\t\t\t%s\n", my_id, s);
}