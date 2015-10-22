/*http://cmm.ensmp.fr/~willot/tmp/*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "mpi.h"

const int maxn = 12;
const int before = 0;
const int after = 1;

// TODO get
// TODO set

double get(int x, int y, int nprocs) {
	if (x == 0) {
		return beforeLine[y];
	}
	else if (x == maxn/nprocs+1) {
		return afterLine[y];
	}
	else {
		
	}
}

int main(int argc, char** argv) {
	int size, rank;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank( MPI_COMM_WORLD, &rank );
	MPI_Comm_size( MPI_COMM_WORLD, &size );

	struct{
		double beforeLine[maxn];
		double tab[maxn/size][maxn];
		double afterLine[maxn];
	} xlocal;

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
		for ( i = 0; i < maxn/size+2; i++) {
			for (j = 0; j < maxn; j++) {
				fnyu = 0.25 * (
					get(i+1, j) + get(i-1, j) + get(i, j+1) + get(i, j-1)
				);
				err += fnyu - get(i, j);
				err *= err;
				set(i, j, fnyu);
			}
		}
		err = sqrt(err);
	}
	
	MPI_Finalize();
	
	return EXIT_SUCCESS;
}
