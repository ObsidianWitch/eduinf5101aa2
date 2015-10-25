#include <stdlib.h>
#include <stdio.h>
#include "mpi.h"
#include "local_matrix.h"
#include "array2D.h"

const int BEFORE_TAG = 0;
const int AFTER_TAG = 1;

struct LocalMatrix createLocalMatrix(int nprocs, int nmatrix) {
    int lines = nmatrix/nprocs;
    int cols = nmatrix;
    
    struct LocalMatrix localMatrix;
    localMatrix.beforeLine = malloc(cols * sizeof(double));
    localMatrix.afterLine = malloc(cols * sizeof(double));
    localMatrix.matrix = malloc2D(lines, cols);
    
    return localMatrix;
}

void destructLocalMatrix(struct LocalMatrix* matrix) {
    free(matrix->beforeLine);
    free(matrix->afterLine);
    free2D(matrix->matrix);
}

/**
 * Initializes localMatrix.
 * first process: initializes beforeLine with -1
 * last process: initializes afterLine with -1
 * all processes: initializes inner matrix with rank
 */
void localInitialization(
	struct LocalMatrix* matrix, int nprocs, int nmatrix, int rank
) {
	fillInner(matrix, nprocs, nmatrix, rank);
	if (rank == 0) {
		fillBeforeLine(matrix, nmatrix, -1);
	}
	else if (rank == nprocs - 1) {
		fillAfterLine(matrix, nmatrix, -1);
	}
}

/**
 * Initializes localMatrix's afterLine & beforeLine by sending and receiving
 * data from other processes using MPI.
 */
void remoteInitialization(
	struct LocalMatrix* matrix, int nprocs, int nmatrix, int rank
) {
	if (rank == 0) {
		sendLastToBeforeLine(matrix, nprocs, nmatrix, rank);
		recvAfterFromFirstLine(matrix, nmatrix, rank);
	}
	else if (rank == nprocs - 1) {
		sendFirstToAfterLine(matrix, nmatrix, rank);
		recvBeforeFromLastLine(matrix, nmatrix, rank);
	}
	else {
		sendLastToBeforeLine(matrix, nprocs, nmatrix, rank);
		sendFirstToAfterLine(matrix, nmatrix, rank);
		recvAfterFromFirstLine(matrix, nmatrix, rank);
		recvBeforeFromLastLine(matrix, nmatrix, rank);
	}
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
 * Fills beforeLine with value.
 */
void fillBeforeLine(struct LocalMatrix* matrix, int nmatrix, double value) {
    for (int j = 0 ; j < nmatrix ; j++) {
        matrix->beforeLine[j] = value;
    }
}

/**
 * Fills afterLine with value.
 */
void fillAfterLine(struct LocalMatrix* matrix, int nmatrix, double value) {
    for (int j = 0 ; j < nmatrix ; j++) {
        matrix->afterLine[j] = value;
    }
}

/**
 * Sends the last line from the current process' inner matrix to the beforeLine
 * of the next process
 */
void sendLastToBeforeLine(
    struct LocalMatrix* matrix, int nprocs, int nmatrix, int currentRank
) {
    int i = nmatrix/nprocs - 1;
    
    MPI_Send(
        matrix->matrix[i], nmatrix, MPI_DOUBLE,
        currentRank + 1, BEFORE_TAG, MPI_COMM_WORLD
    );
}

/**
 * Sends the first line from the current process' inner matrix to the afterLine
 * of the previous process
 */
void sendFirstToAfterLine(
    struct LocalMatrix* matrix, int nmatrix, int currentRank
) {
    MPI_Send(
        matrix->matrix[0], nmatrix, MPI_DOUBLE,
        currentRank - 1, AFTER_TAG, MPI_COMM_WORLD
    );
}

/**
 * Receives the first line from the next process inner matrix and stores it in
 * the LocalMatrix's afterLine.
 */
void recvAfterFromFirstLine(
    struct LocalMatrix* matrix, int nmatrix, int currentRank
) {
    MPI_Recv(
        matrix->afterLine, nmatrix, MPI_DOUBLE,
        currentRank + 1, AFTER_TAG,
        MPI_COMM_WORLD, MPI_STATUS_IGNORE
    );
}

/**
 * Receives the last line from the previous process inner matrix and stores it
 * in the LocalMatrix's beforeLine.
 */
void recvBeforeFromLastLine(
    struct LocalMatrix* matrix, int nmatrix, int currentRank
) {
    MPI_Recv(
        matrix->beforeLine, nmatrix, MPI_DOUBLE,
        currentRank - 1, BEFORE_TAG,
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
