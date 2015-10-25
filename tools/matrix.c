#include <stdlib.h>
#include "matrix.h"

/**
 * Allocates a 2D array.
 * @param lines
 * @param cols
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
 * @param array
 */
void free2D(double** array) {
	free(array[0]);
	free(array);
}

/**
 * Retrieves the (x,y) element inside the specified matrix.
 * @param matrix Instance of the LocalMatrix struct containing an inner matrix,
 * one line before, and one line after.
 * @param nprocs Number of processes currently running this program.
 * @param nmatrix Size of the matrix resulting from the combination of the
 * local matrices on all the processes.
 * @param x
 * @param y
 */
double get(struct LocalMatrix* matrix, int nprocs, int nmatrix, int x, int y) {
    int lines = nmatrix/nprocs + 2;
    
    if (x == 0) {
        return matrix->beforeLine[y];
    }
    else if (x == lines - 1) {
        return matrix->afterLine[y];
    }
    else {
        return matrix->matrix[x - 1][y];
    }
}

/**
 * Sets a value to the (x,y) element inside the specified matrix.
 * @param matrix Instance of the LocalMatrix struct containing an inner matrix,
 * one line before, and one line after.
 * @param nprocs Number of processes currently running this program.
 * @param nmatrix Size of the matrix resulting from the combination of the
 * local matrices on all the processes.
 * @param x
 * @param y
 * @param value
 */
void set(
    struct LocalMatrix* matrix, int nprocs, int nmatrix,
    int x, int y, double value
) {
    int lines = nmatrix/nprocs + 2;
    
    if (x == 0) {
        matrix->beforeLine[y] = value;
    }
    else if (x == lines - 1) {
        matrix->afterLine[y] = value;
    }
    else {
        matrix->matrix[x - 1][y] = value;
    }
}

/**
 * Fills the matrix contained in the specified LocalMatrix instance. Do not
 * modifiy the values inside beforeLine and afterLine.
 * @param matrix Instance of the LocalMatrix struct containing an inner matrix,
 * one line before, and one line after.
 * @param nprocs Number of processes currently running this program.
 * @param nmatrix Size of the matrix resulting from the combination of the
 * local matrices on all the processes.
 * @param value
 */
void fillInner(
    struct LocalMatrix* matrix, int nprocs, int nmatrix, double value
) {
    int lines = nmatrix/nprocs;
    
    for (int i = 0 ; i < lines ; i++) {
        for (int j = 0 ; j < nmatrix ; j++) {
            matrix->matrix[i][j] = value;
        }
    }
}
