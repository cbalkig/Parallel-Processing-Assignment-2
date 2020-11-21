//
// Created by Cavide Balki Gemirter on 20.11.2020.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    false, true
} bool;

void printMatrix(char *id, int rowCount, int columnCount, int matrix[rowCount][columnCount]) {
    char *s = (char *) malloc(1000 * sizeof(char));
    sprintf(s, "%s\n", id);
    for (int row = 0; row < rowCount; row++) {
        sprintf(s, "%s\t\t\t\t\t", s);
        for (int columns = 0; columns < columnCount; columns++) {
            sprintf(s, "%s%d\t", s, matrix[row][columns]);
        }
        sprintf(s, "%s\n", s);
    }
    printf(s);
    free(s);
}

void logTime(char *id, clock_t start, clock_t end) {
    char *s = (char *) malloc(1000 * sizeof(char));
    sprintf(s, "%s", id);
    double cpu_time_used;
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    sprintf(s, "%s%f seconds\n", s, cpu_time_used);
    printf(s);
    free(s);
}