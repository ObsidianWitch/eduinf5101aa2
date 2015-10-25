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

LocalMatrix createLocalMatrix(int nprocs, int nmatrix);
void destructLocalMatrix(LocalMatrix* matrix);

void localInitialization(LocalMatrix* matrix, int nprocs, int rank);
void remoteInitialization(LocalMatrix* matrix, int nprocs, int rank);

double get(LocalMatrix* matrix, int x, int y);
void set(LocalMatrix* matrix, int x, int y, double value);

void fillInner(LocalMatrix* matrix, double value);
void fillBeforeLine(LocalMatrix* matrix, double value);
void fillAfterLine(LocalMatrix* matrix, double value);

void sendLastToBeforeLine(LocalMatrix* matrix, int currentRank);
void sendFirstToAfterLine(LocalMatrix* matrix, int currentRank);
void recvAfterFromFirstLine(LocalMatrix* matrix, int currentRank);
void recvBeforeFromLastLine(LocalMatrix* matrix, int currentRank);

void display(LocalMatrix* matrix);
void writeMatrix(LocalMatrix* matrix, char* fileName);
void WriteFullMatrix(LocalMatrix* matrix, int nprocs, int rank);

#endif // MATRIX
