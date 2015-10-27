#ifndef MATRIX2D
#define MATRIX2D

#include <stdbool.h>
#include "rank2D.h"

typedef struct LocalMatrix {
    double* beforeLine;
    double* beforeCol;
    double** matrix;
    double* afterLine;
    double* afterCol;
    
    int innerSize;
    int totalSize;
} LocalMatrix;

LocalMatrix createLocalMatrix(int nprocs, int nmatrix, Rank2D rank2D);
void destructLocalMatrix(LocalMatrix* matrix);

void localInitialization(
    LocalMatrix* matrix, int nprocs, int nmatrix, Rank2D rank2D
);

bool corner(LocalMatrix* matrix, int i, int j);
double get(LocalMatrix* matrix, int i, int j);
void set(LocalMatrix* matrix, int i, int j, double value);

void writeMatrixLine(
    LocalMatrix* matrix, char* fileName, int i, bool boundaries, bool endline
);
void writeFullMatrix(
    LocalMatrix* matrix, int nprocs, Rank2D rank2D, bool boundaries
);

#endif // MATRIX2D
