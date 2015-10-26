#include <stdlib.h>
#include <stdio.h>
#include "array2D.h"

/**
 * Allocates a 2D array.
 */
double** malloc2D(int lines, int cols) {
    double*  arrayBlock = malloc(lines * cols * sizeof(double));
    double** arrayLines = malloc(lines * sizeof(double*));
    
    for (int i = 0 ; i < lines ; i++) {
    	arrayLines[i] = &arrayBlock[i*cols];
    }
    
    return arrayLines;
}

/**
 * Frees a 2D array.
 */
void free2D(double** array) {
	free(array[0]);
	free(array);
}

/**
 * Prints a 2D array.
 */
void print2D(double** array, int lines, int cols) {
    for (int i = 0 ; i < lines ; i++) {
        for (int j = 0 ; j < cols ; j++) {
            printf("%6.3f ", array[i][j]);
        }
        printf("\n");
    }
}
