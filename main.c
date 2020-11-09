#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>

void readFile(char name, float x[100], int my_id) {
    FILE *file;

    file = fopen(name, "r");
    if (file == NULL) {
        printf("Process %d:\t\t\tFile read error!\n", my_id);
        exit(-1);
    }

    for(int i=0; i<100; i++) {
        fscanf(file, "%f*s", &x[i]);
    }
    fclose(file);
}

int main(int argc, char *argv[]) {
    // Some logging
    printf("Program started.\n");

    // Declarations
    char fileName = "indata.txt";
    int size = 100;
    int process_count, my_id;
    char my_name[MPI_MAX_PROCESSOR_NAME];
    int my_name_len;
    int err;
    MPI_Status status;
    float x[size], sum, sum0;
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
        readFile(fileName, x, my_id);
        printf("Process %d:\t\t\tThe file is closed.\n", my_id);

        if (process_count > 1) {
            for (int i = 1; i < process_count; i++) {
                err = MPI_Send(&x[i * slider], slider, MPI_FLOAT, i, 0, MPI_COMM_WORLD);
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
        err = MPI_Recv(&x[0], slider, MPI_FLOAT, root, 0, MPI_COMM_WORLD, &status);
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
        sum += x[i];
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