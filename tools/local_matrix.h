#ifndef MATRIX
#define MATRIX

typedef struct LocalMatrix {
    double* beforeLine;
    double** matrix;
    double* afterLine;

    int innerLines;
    int totalLines;
    int cols;
} LocalMatrix;

LocalMatrix createLocalMatrix(int nprocs, int nmatrix, int rank);
void destructLocalMatrix(LocalMatrix* matrix);

void localInitialization(LocalMatrix* matrix, int nprocs, int rank);
void updateBoundaries(LocalMatrix* matrix, int nprocs, int rank);

double get(LocalMatrix* matrix, int i, int j);
void set(LocalMatrix* matrix, int i, int j, double value);

void fillInner(LocalMatrix* matrix, double value);
void fillInnerLine(LocalMatrix* matrix, int i, double value);
void fillBeforeLine(LocalMatrix* matrix, double value);
void fillAfterLine(LocalMatrix* matrix, double value);

void sendLastToBeforeLine(LocalMatrix* matrix, int rank);
void sendFirstToAfterLine(LocalMatrix* matrix, int rank);
void recvAfterFromFirstLine(LocalMatrix* matrix, int rank);
void recvBeforeFromLastLine(LocalMatrix* matrix, int rank);

void writeMatrix(LocalMatrix* matrix, char* fileName);
void WriteFullMatrix(LocalMatrix* matrix, int nprocs, int rank);

#endif // MATRIX
