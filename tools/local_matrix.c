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
    
    localMatrix.innerLines = lines;
    localMatrix.totalLines = lines + 2;
    localMatrix.cols = cols;
    
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
void localInitialization(struct LocalMatrix* matrix, int nprocs, int rank) {
    fillInner(matrix, rank);
    if (rank == 0) {
        fillBeforeLine(matrix, -1);
    }
    else if (rank == nprocs - 1) {
        fillAfterLine(matrix, -1);
    }
}

/**
 * Initializes localMatrix's afterLine & beforeLine by sending and receiving
 * data from other processes using MPI.
 */
void remoteInitialization(struct LocalMatrix* matrix, int nprocs, int rank) {
    if (rank == 0) {
        sendLastToBeforeLine(matrix, rank);
        recvAfterFromFirstLine(matrix, rank);
    }
    else if (rank == nprocs - 1) {
        sendFirstToAfterLine(matrix, rank);
        recvBeforeFromLastLine(matrix, rank);
    }
    else {
        sendLastToBeforeLine(matrix, rank);
        sendFirstToAfterLine(matrix, rank);
        recvAfterFromFirstLine(matrix, rank);
        recvBeforeFromLastLine(matrix, rank);
    }
}

/**
 * Retrieves the (x,y) element inside the specified matrix.
 * @param matrix Instance of the LocalMatrix struct containing an inner matrix,
 * one line before, and one line after.
 * @param x
 * @param y
 */
double get(struct LocalMatrix* matrix, int x, int y) {
    if (x == 0) {
        return matrix->beforeLine[y];
    }
    else if (x == matrix->totalLines - 1) {
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
 * @param x
 * @param y
 * @param value
 */
void set(struct LocalMatrix* matrix, int x, int y, double value) {
    if (x == 0) {
        matrix->beforeLine[y] = value;
    }
    else if (x == matrix->totalLines - 1) {
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
 * @param value
 */
void fillInner(struct LocalMatrix* matrix, double value) {
    for (int i = 0 ; i < matrix->innerLines ; i++) {
        for (int j = 0 ; j < matrix->cols ; j++) {
            matrix->matrix[i][j] = value;
        }
    }
}

/**
 * Fills beforeLine with value.
 */
void fillBeforeLine(struct LocalMatrix* matrix, double value) {
    for (int j = 0 ; j < matrix->cols ; j++) {
        matrix->beforeLine[j] = value;
    }
}

/**
 * Fills afterLine with value.
 */
void fillAfterLine(struct LocalMatrix* matrix, double value) {
    for (int j = 0 ; j < matrix->cols ; j++) {
        matrix->afterLine[j] = value;
    }
}

/**
 * Sends the last line from the current process' inner matrix to the beforeLine
 * of the next process
 */
void sendLastToBeforeLine(struct LocalMatrix* matrix, int currentRank) {
    int i = matrix->innerLines - 1;
    
    MPI_Send(
        matrix->matrix[i], matrix->cols, MPI_DOUBLE,
        currentRank + 1, BEFORE_TAG, MPI_COMM_WORLD
    );
}

/**
 * Sends the first line from the current process' inner matrix to the afterLine
 * of the previous process
 */
void sendFirstToAfterLine(struct LocalMatrix* matrix, int currentRank) {
    MPI_Send(
        matrix->matrix[0], matrix->cols, MPI_DOUBLE,
        currentRank - 1, AFTER_TAG, MPI_COMM_WORLD
    );
}

/**
 * Receives the first line from the next process inner matrix and stores it in
 * the LocalMatrix's afterLine.
 */
void recvAfterFromFirstLine(struct LocalMatrix* matrix, int currentRank) {
    MPI_Recv(
        matrix->afterLine, matrix->cols, MPI_DOUBLE,
        currentRank + 1, AFTER_TAG,
        MPI_COMM_WORLD, MPI_STATUS_IGNORE
    );
}

/**
 * Receives the last line from the previous process inner matrix and stores it
 * in the LocalMatrix's beforeLine.
 */
void recvBeforeFromLastLine(struct LocalMatrix* matrix, int currentRank) {
    MPI_Recv(
        matrix->beforeLine, matrix->cols, MPI_DOUBLE,
        currentRank - 1, BEFORE_TAG,
        MPI_COMM_WORLD, MPI_STATUS_IGNORE
    );
}

/**
 * Displays the matrix.
 * @param matrix Instance of the LocalMatrix struct containing an inner matrix,
 * one line before, and one line after.
 */
void display(struct LocalMatrix* matrix) {
    for (int i = 0 ; i < matrix->totalLines ; i++) {
        for (int j = 0 ; j < matrix->cols ; j++) {
            double value = get(matrix, i, j);
            printf("%2.0f ", value);
        }
        printf("\n");
    }
}
