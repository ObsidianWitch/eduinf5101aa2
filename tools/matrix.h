#ifndef MATRIX
#define MATRIX

struct LocalMatrix {
	double* beforeLine;
	double** matrix;
	double* afterLine;
};

double** malloc2D(int lines, int cols);
void free2D(double** array);

struct LocalMatrix createLocalMatrix(int nprocs, int nmatrix);
void localInitialization(
	struct LocalMatrix* matrix, int nprocs, int nmatrix, int rank
);
void remoteInitialization(
	struct LocalMatrix* matrix, int nprocs, int nmatrix, int rank
);

double get(struct LocalMatrix* matrix, int nprocs, int nmatrix, int x, int y);
void set(
    struct LocalMatrix* matrix, int nprocs, int nmatrix,
    int x, int y, double value
);

void fillInner(
    struct LocalMatrix* matrix, int nprocs, int nmatrix, double value
);
void fillBeforeLine(struct LocalMatrix* matrix, int nmatrix, double value);
void fillAfterLine(struct LocalMatrix* matrix, int nmatrix, double value);

void sendLastToBeforeLine(
    struct LocalMatrix* matrix, int nprocs, int nmatrix, int currentRank
);
void sendFirstToAfterLine(
    struct LocalMatrix* matrix, int nmatrix, int currentRank
);
void recvAfterFromFirstLine(
    struct LocalMatrix* matrix, int nmatrix, int currentRank
);
void recvBeforeFromLastLine(
    struct LocalMatrix* matrix, int nmatrix, int currentRank
);

void display(struct LocalMatrix* matrix, int nprocs, int nmatrix);

#endif // MATRIX
