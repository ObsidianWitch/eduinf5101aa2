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

/**
 * Fills a 2D array with value.
 */
void fill2D(double** array, int lines, int cols, double value) {
    for (int i = 0 ; i < lines ; i++) {
        for (int j = 0 ; j < cols ; j++) {
            array[i][j] = value;
        }
    }
}

/**
 * Sequentially fills a 1D array with a value (e.g. fill a line).
 */
void fillSeq(double* array, int size, double value) {
    for (int k = 0 ; k < size ; k++) {
        array[k] = value;
    }
}

/**
 * Fills a 2D array's column.
 */
void fillCol(double** array, int lines, int j, double value) {
    for (int i = 0 ; i < lines ; i++) {
        array[i][j] = value;
    }
}
