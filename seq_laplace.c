#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "tools/array2D.h"

const int N_MATRIX = 12;
const int N_SUBMATRICES = 4;
const float DELTA = 1.0e-6;

typedef struct Matrix {
    double** array;
    int size;
} Matrix;

Matrix constructMatrix() {
    Matrix matrix = {
        .array = malloc2D(N_MATRIX, N_MATRIX),
        .size  = N_MATRIX
    };
    
    for (int i = 0 ; i < matrix.size ; i++) {
        for (int j = 0 ; j < matrix.size ; j++) {
            if (i == 0 || i == matrix.size - 1) {
                matrix.array[i][j] = -1;
            }
            else {
                double value = i / (matrix.size / N_SUBMATRICES);
                matrix.array[i][j] = value;
            }
        }
    }
    
    return matrix;
}

void computeLaplaceEquation(Matrix* matrix) {
    double err = 1.0;
    while (err > DELTA) {
        err = 0;
        for (int i = 1 ; i < matrix->size - 1 ; i++) {
            for (int j = 1 ; j < matrix->size - 1 ; j++) {
                double newValue = 0.25 * (
                    matrix->array[i + 1][j] +
                    matrix->array[i - 1][j] +
                    matrix->array[i][j + 1] +
                    matrix->array[i][j - 1]
                );
                
                err += newValue - matrix->array[i][j];
                err *= err;
                matrix->array[i][j] = newValue;
            }
        }
        err = sqrt(err);
    }
}

void printMatrix(Matrix* matrix) {
    print2D(matrix->array, matrix->size, matrix->size);
}

int main() {
    Matrix matrix = constructMatrix();
    
    computeLaplaceEquation(&matrix);
    
    printMatrix(&matrix);
    
    free2D(matrix.array);

    return EXIT_SUCCESS;
}
