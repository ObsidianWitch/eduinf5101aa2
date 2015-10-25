#ifndef MATRIX
#define MATRIX

struct LocalMatrix {
	double* beforeLine;
	double** matrix;
	double* afterLine;
};

double** malloc2D(int lines, int cols);
void free2D(double** array);

double get(struct LocalMatrix* matrix, int nprocs, int nmatrix, int x, int y);
void set(
    struct LocalMatrix* matrix, int nprocs, int nmatrix,
    int x, int y, double value
);
void fillInner(
    struct LocalMatrix* matrix, int nprocs, int nmatrix, double value
);
void sendLine(
    struct LocalMatrix* matrix, int nprocs, int nmatrix,
    int x, int rank, int tag
);
void receiveLine(
    struct LocalMatrix* matrix, int nprocs, int nmatrix,
    int x, int rank, int tag
);

#endif // MATRIX
