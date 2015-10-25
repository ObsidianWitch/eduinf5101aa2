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

#endif // MATRIX
