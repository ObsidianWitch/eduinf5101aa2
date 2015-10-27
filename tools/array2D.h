#ifndef ARRAY2D
#define ARRAY2D

double** malloc2D(int lines, int cols);
void free2D(double** array);
void print2D(double** array, int lines, int cols);

void fill2D(double** array, int lines, int cols, double value);
void fillSeq(double* array, int size, double value);
void fillCol(double** array, int lines, int j, double value);

#endif // ARRAY2D
