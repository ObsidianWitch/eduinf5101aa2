#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include "mpi.h"
#include "local_matrix.h"
#include "array2D.h"

const int BEFORE_TAG = 0;
const int AFTER_TAG = 1;
const int DISPLAY_TAG = 2;

/**
 * Creates and initializes a LocalMatrix.
 */
LocalMatrix createLocalMatrix(int nprocs, int nmatrix, int rank) {
    int lines = nmatrix/nprocs;
    int cols = nmatrix;
    
    LocalMatrix localMatrix;
    localMatrix.beforeLine = malloc(cols * sizeof(double));
    localMatrix.afterLine = malloc(cols * sizeof(double));
    localMatrix.matrix = malloc2D(lines, cols);
    
    localMatrix.innerLines = lines;
    localMatrix.totalLines = lines + 2;
    localMatrix.cols = cols;
    
    localInitialization(&localMatrix, nprocs, rank);
    updateBoundaries(&localMatrix, nprocs, rank);
    
    return localMatrix;
}

/**
 * Frees allocated memory for a LocalMatrix.
 */
void destructLocalMatrix(LocalMatrix* matrix) {
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
void localInitialization(LocalMatrix* matrix, int nprocs, int rank) {
    fillInner(matrix, rank);
    
    if (rank == 0) {
        fillInnerLine(matrix, 0, -1);
        fillBeforeLine(matrix, -2);
    }
    else if (rank == nprocs - 1) {
        fillInnerLine(matrix, matrix->innerLines - 1, -1);
        fillAfterLine(matrix, -2);
    }
}

/**
 * Updates localMatrix's afterLine & beforeLine by sending and receiving
 * data from other processes using MPI.
 */
void updateBoundaries(LocalMatrix* matrix, int nprocs, int rank) {
    if (rank > 0) {
        sendFirstToAfterLine(matrix, rank);
        recvBeforeFromLastLine(matrix, rank);
    }
    
    if (rank < nprocs - 1) {
        sendLastToBeforeLine(matrix, rank);
        recvAfterFromFirstLine(matrix, rank);
    }
}

/**
 * Retrieves the (i,j) element inside the specified matrix.
 */
double get(LocalMatrix* matrix, int i, int j) {
    if (i == 0) {
        return matrix->beforeLine[j];
    }
    else if (i == matrix->totalLines - 1) {
        return matrix->afterLine[j];
    }
    else {
        return matrix->matrix[i - 1][j];
    }
}

/**
 * Sets a value to the (i,j) element inside the specified matrix.
 */
void set(LocalMatrix* matrix, int i, int j, double value) {
    if (i == 0) {
        matrix->beforeLine[j] = value;
    }
    else if (i == matrix->totalLines - 1) {
        matrix->afterLine[j] = value;
    }
    else {
        matrix->matrix[i - 1][j] = value;
    }
}

/**
 * Fills the matrix contained in the specified LocalMatrix instance. Do not
 * modify the values inside beforeLine and afterLine.
 */
void fillInner(LocalMatrix* matrix, double value) {
    for (int i = 0 ; i < matrix->innerLines ; i++) {
        for (int j = 0 ; j < matrix->cols ; j++) {
            matrix->matrix[i][j] = value;
        }
    }
}

/**
 * Fills line with value.
 */
void fillInnerLine(LocalMatrix* matrix, int i, double value) {
    for (int j = 0 ; j < matrix->cols ; j++) {
        matrix->matrix[i][j] = value;
    }
}

/**
 * Fills beforeLine with value.
 */
void fillBeforeLine(LocalMatrix* matrix, double value) {
    for (int j = 0 ; j < matrix->cols ; j++) {
        matrix->beforeLine[j] = value;
    }
}

/**
 * Fills afterLine with value.
 */
void fillAfterLine(LocalMatrix* matrix, double value) {
    for (int j = 0 ; j < matrix->cols ; j++) {
        matrix->afterLine[j] = value;
    }
}

/**
 * Sends the last line from the current process' inner matrix to the beforeLine
 * of the next process
 */
void sendLastToBeforeLine(LocalMatrix* matrix, int rank) {
    int i = matrix->innerLines - 1;
    
    MPI_Send(
        matrix->matrix[i], matrix->cols, MPI_DOUBLE,
        rank + 1, BEFORE_TAG, MPI_COMM_WORLD
    );
}

/**
 * Sends the first line from the current process' inner matrix to the afterLine
 * of the previous process
 */
void sendFirstToAfterLine(LocalMatrix* matrix, int rank) {
    MPI_Send(
        matrix->matrix[0], matrix->cols, MPI_DOUBLE,
        rank - 1, AFTER_TAG, MPI_COMM_WORLD
    );
}

/**
 * Receives the first line from the next process inner matrix and stores it in
 * the LocalMatrix's afterLine.
 */
void recvAfterFromFirstLine(LocalMatrix* matrix, int rank) {
    MPI_Recv(
        matrix->afterLine, matrix->cols, MPI_DOUBLE,
        rank + 1, AFTER_TAG,
        MPI_COMM_WORLD, MPI_STATUS_IGNORE
    );
}

/**
 * Receives the last line from the previous process inner matrix and stores it
 * in the LocalMatrix's beforeLine.
 */
void recvBeforeFromLastLine(LocalMatrix* matrix, int rank) {
    MPI_Recv(
        matrix->beforeLine, matrix->cols, MPI_DOUBLE,
        rank - 1, BEFORE_TAG,
        MPI_COMM_WORLD, MPI_STATUS_IGNORE
    );
}

/**
 * Writes the specified LocalMatrix to a file. Boundaries can be written by
 * setting the associated parameter to true.
 */
void writeMatrix(LocalMatrix* matrix, char* fileName, bool boundaries) {
    FILE* f = fopen(fileName, "a");
    if (f == NULL) { perror("fopen"); }
    
    int iStart = 0;
    int iEnd = matrix->totalLines;
    
    if (!boundaries) { iStart++; iEnd--; }
    
    for (int i = iStart ; i < iEnd ; i++) {
        for (int j = 0 ; j < matrix->cols ; j++) {
            double value = get(matrix, i, j);
            fprintf(f, "%6.3f ", value);
        }
        fprintf(f, "\n");
    }
    
    fclose(f);
}

/**
 * Writes the full matrix by telling each process to write its LocalMatrix in
 * a file. The processes write in the correct order by using a token ring.
 * Boundaries can be written by setting the associated parameter to true.
 */
void writeFullMatrix(LocalMatrix* matrix, int nprocs, int rank, bool boundaries) {
    if (rank == 0) { remove("matrix.out"); }
    
    if (rank != 0) {
        int tmp = 0;
        MPI_Recv(
            &tmp, 1, MPI_INT, rank - 1, DISPLAY_TAG,
            MPI_COMM_WORLD, MPI_STATUS_IGNORE
        );
    }
    
    writeMatrix(matrix, "matrix.out", boundaries);
    
    if (rank != nprocs - 1) {
        int tmp = 0;
        MPI_Send(&tmp, 1, MPI_INT, rank + 1, DISPLAY_TAG, MPI_COMM_WORLD);
    }
}
