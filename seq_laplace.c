#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "tools/array2D.h"

typedef struct Matrix {
    double** array;
    int size;
} Matrix;

Matrix constructMatrix(int nmatrix, int nsubmatrices) {
    Matrix matrix = {
        .array = malloc2D(nmatrix, nmatrix),
        .size  = nmatrix
    };
    
    for (int i = 0 ; i < matrix.size ; i++) {
        for (int j = 0 ; j < matrix.size ; j++) {
            if (i == 0 || i == matrix.size - 1) {
                matrix.array[i][j] = -1;
            }
            else {
                double value = i / (matrix.size / nsubmatrices);
                matrix.array[i][j] = value;
            }
        }
    }
    
    return matrix;
}

void computeLaplaceEquation(Matrix* matrix, float delta) {
    double err = 1.0;
    while (err > delta) {
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

void writeMatrix(Matrix* matrix) {
    remove("matrix.out");
    FILE* f = fopen("matrix.out", "a");
    
    for (int i = 0 ; i < matrix->size ; i++) {
        for (int j = 0 ; j < matrix->size ; j++) {
            fprintf(f, "%6.3f ", matrix->array[i][j]);
        }
        fprintf(f, "\n");
    }
    
    fclose(f);
}

int main(int argc, char** argv) {
    if (argc < 4) {
        printf(
            "Missing arguments\n"
            "Usage : %s N_MATRIX N_SUBMATRICES DELTA\n",
            argv[0]
        );
        return EXIT_FAILURE;
    }
    int nmatrix = atoi(argv[1]);
    int nsubmatrices = atoi(argv[2]);
    float delta = atof(argv[3]);
    
    Matrix matrix = constructMatrix(nmatrix, nsubmatrices);
    
    computeLaplaceEquation(&matrix, delta);
    
    writeMatrix(&matrix);
    
    free2D(matrix.array);

    return EXIT_SUCCESS;
}
