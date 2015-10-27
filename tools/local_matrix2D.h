#ifndef MATRIX2D
#define MATRIX2D

#include <stdbool.h>

typedef struct LocalMatrix {
    double* beforeLine;
    double* beforeCol;
    double** matrix;
    double* afterLine;
    double* afterCol;
    
    int innerSize;
    int totalSize;
} LocalMatrix;

LocalMatrix createLocalMatrix(int nprocs, int nmatrix, int rank);
void destructLocalMatrix(LocalMatrix* matrix);

void localInitialization(LocalMatrix* matrix, int nprocs, int nmatrix, int rank);

bool corner(LocalMatrix* matrix, int i, int j);
double get(LocalMatrix* matrix, int i, int j);
void set(LocalMatrix* matrix, int i, int j, double value);

void writeMatrix(LocalMatrix* matrix, char* fileName, bool boundaries);
void writeFullMatrix(LocalMatrix* matrix, int nprocs, int rank, bool boundaries);

#endif // MATRIX2D
