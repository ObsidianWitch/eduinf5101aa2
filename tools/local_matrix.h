#ifndef MATRIX
#define MATRIX

struct LocalMatrix {
    double* beforeLine;
    double** matrix;
    double* afterLine;

    int innerLines;
    int totalLines;
    int cols;
};

struct LocalMatrix createLocalMatrix(int nprocs, int nmatrix);
void destructLocalMatrix(struct LocalMatrix* matrix);

void localInitialization(struct LocalMatrix* matrix, int nprocs, int rank);
void remoteInitialization(struct LocalMatrix* matrix, int nprocs, int rank);

double get(struct LocalMatrix* matrix, int x, int y);
void set(struct LocalMatrix* matrix, int x, int y, double value);

void fillInner(struct LocalMatrix* matrix, double value);
void fillBeforeLine(struct LocalMatrix* matrix, double value);
void fillAfterLine(struct LocalMatrix* matrix, double value);

void sendLastToBeforeLine(struct LocalMatrix* matrix, int currentRank);
void sendFirstToAfterLine(struct LocalMatrix* matrix, int currentRank);
void recvAfterFromFirstLine(struct LocalMatrix* matrix, int currentRank);
void recvBeforeFromLastLine(struct LocalMatrix* matrix, int currentRank);

void display(struct LocalMatrix* matrix);

#endif // MATRIX
