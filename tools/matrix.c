#include <stdlib.h>
#include <stdio.h>
#include "matrix.h"
#include "mpi.h"

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

struct LocalMatrix createLocalMatrix(int nprocs, int nmatrix) {
    int lines = nmatrix/nprocs;
    int cols = nmatrix;
    
    struct LocalMatrix localMatrix;
    localMatrix.beforeLine = malloc(cols * sizeof(double));
    localMatrix.afterLine = malloc(cols * sizeof(double));
    localMatrix.matrix = malloc2D(lines, cols);
    
    return localMatrix;
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
 * modify the values inside beforeLine and afterLine.
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

/**
 * Sends a line to another process using MPI.
 * @param matrix Instance of the LocalMatrix struct containing an inner matrix,
 * one line before, and one line after.
 * @param nprocs Number of processes currently running this program.
 * @param nmatrix Size of the matrix resulting from the combination of the
 * local matrices on all the processes.
 * @param x Index of the line to send.
 * @param rank Rank of the receiver.
 * @param tag Mesasge tag.
 */
void sendLine(
    struct LocalMatrix* matrix, int nprocs, int nmatrix,
    int x, int rank, int tag
) {
    int lines = nmatrix/nprocs + 2;
    double* line = NULL;
    
    if (x == 0) {
        line = matrix->beforeLine;
    }
    else if (x == lines - 1) {
        line = matrix->afterLine;
    }
    else {
        line = matrix->matrix[x];
    }
    
    MPI_Send(line, nmatrix, MPI_DOUBLE, rank, tag, MPI_COMM_WORLD);
}

/**
 * Receives a lines from another process using MPI.
 * @param matrix Instance of the LocalMatrix struct containing an inner matrix,
 * one line before, and one line after.
 * @param nprocs Number of processes currently running this program.
 * @param nmatrix Size of the matrix resulting from the combination of the
 * local matrices on all the processes.
 * @param x Index of the line to fill with the incoming data.
 * @param rank Rank of the source.
 * @param tag Mesasge tag.
 */
void receiveLine(
    struct LocalMatrix* matrix, int nprocs, int nmatrix,
    int x, int rank, int tag
) {
    int lines = nmatrix/nprocs + 2;
    double* line = NULL;
    
    if (x == 0) {
        line = matrix->beforeLine;
    }
    else if (x == lines - 1) {
        line = matrix->afterLine;
    }
    else {
        line = matrix->matrix[x];
    }
    
    MPI_Recv(
        line, nmatrix, MPI_DOUBLE, rank, tag,
        MPI_COMM_WORLD, MPI_STATUS_IGNORE
    );
}

/**
 * Displays the matrix.
 * @param matrix Instance of the LocalMatrix struct containing an inner matrix,
 * one line before, and one line after.
 * @param nprocs Number of processes currently running this program.
 * @param nmatrix Size of the matrix resulting from the combination of the
 * local matrices on all the processes.
 */
void display(struct LocalMatrix* matrix, int nprocs, int nmatrix) {
    int lines = nmatrix/nprocs + 2;
    
    for (int i = 0 ; i < lines ; i++) {
        for (int j = 0 ; j < nmatrix ; j++) {
            double value = get(matrix, nprocs, nmatrix, i, j);
            printf("%2.0f ", value);
        }
        printf("\n");
    }
}
