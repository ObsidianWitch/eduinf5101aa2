/*http://cmm.ensmp.fr/~willot/tmp/*/

#include <stdio.h>
#include <stdlib.h>

#include "mpi.h"

const int maxn = 12;
const int before = 0;
const int after = 1;

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

	//MPI_Barrier(MPI_COMM_WORLD);
	
	/*
	int done = 0;
	if (rank != 0) {
		MPI_Recv(
			&done, 1, MPI_INT,
			rank-1, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE
		);
	}
	
	printf("{%d}\n", rank);
	for (int i = 0; i < maxn/size; i++) {
		if (i == 0){
			for(int j = 0; j < maxn; j++) {
				printf("%2.0f ", xlocal.beforeLine[j]);
			}
			printf("\n");
		}
		for (int j = 0; j < maxn; j++) {
			printf("%2.0f ", xlocal.tab[i][j]);
		}
		printf("\n");
		if (i == maxn/size-1) {
			for(int j = 0; j < maxn; j++) {
				printf("%2.0f ", xlocal.afterLine[j]);
			}
			printf("\n");
		}
	}
	
	if (rank < size-1) {
		MPI_Send(
			&done, 1, MPI_INT,
			rank+1, 2, MPI_COMM_WORLD
		);
	}*/
	
	MPI_Finalize();
	
	return EXIT_SUCCESS;
}
