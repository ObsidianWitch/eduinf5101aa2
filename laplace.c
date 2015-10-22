/*http://cmm.ensmp.fr/~willot/tmp/*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "mpi.h"

const int maxn = 12;
const int before = 0;
const int after = 1;

struct LocalMatrix{
	double* beforeLine;
	double** tab;
	double* afterLine;
};

// To test get
// TODO set

double get(struct LocalMatrix* matrix, int x, int y, int nprocs) {
	if (x == 0) {
		return matrix->beforeLine[y];
	}
	else if (x == maxn/nprocs+1) {
		return matrix->afterLine[y];
	}
	else {
		return matrix->tab[x-1][y];
	}
}

double** malloc2D(int lines, int cols) {
	double* tabBlock = malloc(lines * cols * sizeof(double));
	double** tabLines = malloc(lines * sizeof(double*));
	
	for (int i = 0; i < lines; i++) {
		tabLines[i] = &tabBlock[i*cols];
	}
	
	return tabLines;
}

void free2D(double** tab){
	free(tab[0]);
	free(tab);
}

int main(int argc, char** argv) {
	int size, rank;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank( MPI_COMM_WORLD, &rank );
	MPI_Comm_size( MPI_COMM_WORLD, &size );

	struct LocalMatrix xlocal;
	xlocal.beforeLine = malloc(maxn * sizeof(double));
	xlocal.afterLine = malloc(maxn * sizeof(double));
	xlocal.tab = malloc2D(maxn/size, maxn);

	for (int i = 0; i < maxn/size; i++) {
		for (int j = 0; j < maxn; j++){
			xlocal.tab[i][j] = rank;
		}
	}

	for (int j = 0; j < maxn; j++) {
		if (rank == 0) {
			xlocal.beforeLine[j] = -1;
			MPI_Send(
				&xlocal.tab[maxn/size-1], maxn, MPI_DOUBLE,
				rank+1, before, MPI_COMM_WORLD
			);
			MPI_Recv(
				&xlocal.afterLine, maxn, MPI_DOUBLE,
				rank+1, after, MPI_COMM_WORLD, MPI_STATUS_IGNORE
			);
		}
		else if (rank == size-1) {
			xlocal.afterLine[j] = -1;
			MPI_Send(
				&xlocal.tab[0], maxn, MPI_DOUBLE,
				rank-1, after, MPI_COMM_WORLD
			);
			MPI_Recv(
				&xlocal.beforeLine, maxn, MPI_DOUBLE,
				rank-1, before, MPI_COMM_WORLD, MPI_STATUS_IGNORE
			);
		}
		else {
			MPI_Send(
				&xlocal.tab[maxn/size-1], maxn, MPI_DOUBLE,
				rank+1, before, MPI_COMM_WORLD
			);
			MPI_Send(
				&xlocal.tab[0], maxn, MPI_DOUBLE,
				rank-1, after, MPI_COMM_WORLD
			);
			MPI_Recv(
				&xlocal.afterLine, maxn, MPI_DOUBLE,
				rank+1, after, MPI_COMM_WORLD, MPI_STATUS_IGNORE
			);
			MPI_Recv(
				&xlocal.beforeLine, maxn, MPI_DOUBLE,
				rank-1, before, MPI_COMM_WORLD, MPI_STATUS_IGNORE
			);
		}
	}
	
	double err = 1.0;
	double delta = 0.001;
	while (err > delta) {
		err = 0;
		for (int i = 0; i < maxn/size+2; i++) {
			for (int j = 0; j < maxn; j++) {
				double fnyu = 0.25 * (
					get(&xlocal, i+1, j, size) +
					get(&xlocal, i-1, j, size) +
					get(&xlocal, i, j+1, size) + 
					get(&xlocal, i, j-1, size)
				);
				err += fnyu - get(&xlocal, i, j, size);
				err *= err;
				//set(i, j, fnyu);
			}
		}
		err = sqrt(err);
	}
	
	free2D(xlocal.tab);
	
	MPI_Finalize();
	
	return EXIT_SUCCESS;
}
