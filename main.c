#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <string.h>

int getLineCount(char name[], int my_id) {
    FILE *file;

    file = fopen(name, "r");
    if (file == NULL) {
        printf("Process %d:\t\t\tFile read error: %s!\n", my_id, name);
        exit(-1);
    }

    int count_lines = 0;
    char chr = chr = getc(file);
    while (chr != EOF)
    {
        if (chr == '\n')
        {
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

    char line[1000];
    for(int i=0; i<size; i++) {
        for(int j=0; j<size; j++) {
            fscanf(file, "%d*s", &line);
            x[i][j] = *line;
        }
    }
    fclose(file);
}

void printMatrix(int size, int matrix[size][size]){
    int row, columns;
    for (row=0; row<size; row++)
    {
        for(columns=0; columns<size; columns++)
        {
            printf("%d\t\t", matrix[row][columns]);
        }
        printf("\n");
    }
}

int main(int argc, char *argv[]) {
    // Some logging
    printf("Block Data Structure - Program started.\n");

    // Declarations
    char matrix1_file_name[] = "/Users/balki/CLionProjects/Assignment-1/matrix1.txt";
    char matrix2_file_name[] = "/Users/balki/CLionProjects/Assignment-1/matrix2.txt";
    int size, my_name_len, process_count, my_id, err, sum, sum0;
    char my_name[MPI_MAX_PROCESSOR_NAME];
    MPI_Status status;
    int matrix1[size][size], matrix2[size][size];
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
    printf("Process %d:\t\t\tInputs:\t\tProcessor Count: %d\t\tMy ID: %d\t\tMy Name: %s\n", my_id, process_count, my_id, my_name);
    int slider = size / process_count;

    // Read or get file content
    if (my_id == root) {
        size = getLineCount(matrix1_file_name, my_id);
        readFile(matrix1_file_name, size, &matrix1, my_id);
        printf("Process %d:\t\t\tMatrix 1 is\n", my_id);
        printMatrix(size, matrix1);
        readFile(matrix2_file_name, size, &matrix2, my_id);
        printf("Process %d:\t\t\tMatrix 2 is\n", my_id);
        printMatrix(size, matrix2);
        printf("Process %d:\t\t\tThe files are closed.\n", my_id);

        if (process_count > 1) {
            for (int i = 1; i < process_count; i++) {
                err = MPI_Send(&matrix1[i * slider], slider, MPI_FLOAT, i, 0, MPI_COMM_WORLD);
                if (err == 0) {
                    printf("Process %d:\t\t\tSend data to worker %d.\n", my_id, i);
                } else {
                    printf("Process %d:\t\t\t!!ERROR: Send data to worker %d: %d.\n", my_id, i, err);
                    exit(-1);
                }
            }
        }
        else{
            printf("Process %d:\t\t\tNo one to send data.\n", my_id);
        }
    } else {
        err = MPI_Recv(&matrix1[0], slider, MPI_FLOAT, root, 0, MPI_COMM_WORLD, &status);
        if (err == 0) {
            printf("Process %d:\t\t\tReceived data from Master.\n", my_id);
        } else {
            printf("Process %d:\t\t\t!!ERROR: Received data from Master: %d.\n", my_id, err);
            exit(-1);
        }
    }

    // Do the calculations
    printf("Process %d:\t\t\tStarted calculations.\n", my_id);
    sum = 0;
    for (int i = 0; i < slider; i++) {
        sum += matrix1[i];
    }
    printf("Process %d:\t\t\tCalculations finished, My Sum=%f\n", my_id, sum);

    // Send or get the calculations.
    if (my_id == root) {
        for (int i = 1; i < process_count; i++) {
            MPI_Recv(&sum0, 1, MPI_FLOAT, i, 0, MPI_COMM_WORLD, &status);
            printf("Process %d:\t\t\tReceived result from %d.\n", my_id, i);
            sum += sum0;
        }
        printf("Process %d:\t\t\tTotal Sum=%f.\n", my_id, sum);
    } else {
        MPI_Send(&sum, 1, MPI_FLOAT, root, 0, MPI_COMM_WORLD);
        printf("Process %d:\t\t\tSent result to Master.\n", my_id);
    }

    MPI_Finalize();
}