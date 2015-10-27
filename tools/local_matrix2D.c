#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <math.h>
#include "mpi.h"
#include "local_matrix2D.h"
#include "array2D.h"
#include "rank2D.h"

const int DISPLAY_TAG = 4;

/**
 * Creates and initializes a LocalMatrix.
 */
LocalMatrix createLocalMatrix(int nprocs, int nmatrix, int rank) {
    int innerSize = nmatrix/sqrt(nprocs);
    int totalSize = innerSize + 2;
    
    LocalMatrix localMatrix;
    localMatrix.beforeLine = malloc(innerSize * sizeof(double));
    localMatrix.beforeCol = malloc(innerSize * sizeof(double));
    localMatrix.matrix = malloc2D(innerSize, innerSize);
    localMatrix.afterLine = malloc(innerSize * sizeof(double));
    localMatrix.afterCol = malloc(innerSize * sizeof(double));
    
    localMatrix.innerSize = innerSize;
    localMatrix.totalSize = totalSize;
    
    localInitialization(&localMatrix, nprocs, nmatrix, rank);
    
    return localMatrix;
}

/**
 * Frees allocated memory for a LocalMatrix.
 */
void destructLocalMatrix(LocalMatrix* matrix) {
    free(matrix->beforeLine);
    free(matrix->beforeCol);
    free2D(matrix->matrix);
    free(matrix->afterLine);
    free(matrix->afterCol);
}

/**
 * Initializes localMatrix.
 */
void localInitialization(LocalMatrix* matrix, int nprocs, int nmatrix, int rank) {
    Rank2D rnk2D = rank2D(nprocs, rank);
    int lastElement = matrix->innerSize - 1;
    
    // fill the matrix with the same values as in the 1D case to easily check
    // whether the results are correct
    for (int i = 0 ; i < matrix->innerSize ; i++) {
        int value = i / (nmatrix / nprocs);
        value += rnk2D.i * sqrt(nprocs);
        fillSeq(matrix->matrix[i], matrix->innerSize, value);
    }
    
    if (rnk2D.i == 0) {
        fillSeq(matrix->matrix[0], matrix->innerSize, -1);
        fillSeq(matrix->beforeLine, matrix->innerSize, -2);
    }
    else if (rnk2D.i == (int) sqrt(nprocs) - 1) {
        fillSeq(matrix->matrix[lastElement], matrix->innerSize, -1);
        fillSeq(matrix->afterLine, matrix->innerSize, -2);
    }
    
    if (rnk2D.j == 0) {
        fillSeq(matrix->beforeCol, matrix->innerSize, -2);
    }
    else if (rnk2D.j == (int) sqrt(nprocs) - 1) {
        fillSeq(matrix->afterCol, matrix->innerSize, -2);
    }
}

/**
 * Checks whether the (i,j) position is a corner.
 */
bool corner(LocalMatrix* matrix, int i, int j) {
    return (i == 0 && j == 0)
        || (i == 0 && j == matrix->totalSize - 1)
        || (i == matrix->totalSize - 1 && j == 0)
        || (i == matrix->totalSize - 1 && j == matrix->totalSize - 1);
}

/**
 * Retrieves the (i,j) element inside the specified matrix.
 */
double get(LocalMatrix* matrix, int i, int j) {
    if (corner(matrix, i, j)) {
        return -4.0;
    }
    else if (i == 0) {
        return matrix->beforeLine[j - 1];
    }
    else if (i == matrix->totalSize - 1) {
        return matrix->afterLine[j - 1];
    }
    else if (j == 0) {
        return matrix->beforeCol[i - 1];
    }
    else if (j == matrix->totalSize - 1) {
        return matrix->afterCol[i - 1];
    }
    else {
        return matrix->matrix[i - 1][j - 1];
    }
}

/**
 * Sets a value to the (i,j) element inside the specified matrix.
 */
void set(LocalMatrix* matrix, int i, int j, double value) {
    if (corner(matrix, i, j)) {
        return;
    }
    else if (i == 0) {
        matrix->beforeLine[j - 1] = value;
    }
    else if (i == matrix->totalSize - 1) {
        matrix->afterLine[j - 1] = value;
    }
    else if (j == 0) {
        matrix->beforeCol[i - 1] = value;
    }
    else if (j == matrix->totalSize - 1) {
        matrix->afterCol[i - 1] = value;
    }
    else {
        matrix->matrix[i - 1][j - 1] = value;
    }
}

/**
 * Writes the specified LocalMatrix to a file. Boundaries can be written by
 * setting the associated parameter to true.
 */
void writeMatrix(LocalMatrix* matrix, char* fileName, bool boundaries) {
    FILE* f = fopen(fileName, "a");
    if (f == NULL) { perror("fopen"); }
    
    int iStart = 0;
    int iEnd = matrix->totalSize;
    int jStart = 0;
    int jEnd = matrix->totalSize;
    
    if (!boundaries) { iStart++; iEnd--; jStart++; jEnd--; }
    
    for (int i = iStart ; i < iEnd ; i++) {
        for (int j = jStart ; j < jEnd ; j++) {
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
 * TODO will write blocks sequentially, the matrix won't be correctly formatted.
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
