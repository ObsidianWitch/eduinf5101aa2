/*http://cmm.ensmp.fr/~willot/tmp/*/

#include <stdio.h>
#include <stdlib.h>

#include "mpi.h"

const int maxn = 12;

int main(int argc, char** argv) {
	int size, rank;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank( MPI_COMM_WORLD, &rank );
	MPI_Comm_size( MPI_COMM_WORLD, &size );

	struct{
		double beforeLine[maxn];
		double tab[maxn][maxn/size];
		double afterLine[maxn];
	} xlocal;

/*void initTab(double** xlocal, int size, int rank){*/
	for (int i = 0; i < maxn; i++) {
		if (rank == 0) {
			xlocal.beforeLine[i] = -1;
		}
		if (rank == size-1) {
			xlocal.afterLine[i] = -2;
		}
		if(rank != 0){
			xlocal.beforeLine[i] = rank-1;
		}
		if(rank != size-1){
			xlocal.afterLine[i] = rank+1;
		}
	}

	for (int i = 0; i < maxn; i++) {
		for (int j = 0; j < maxn/size; j++){
			xlocal.tab[i][j] = rank;
		} 
	}
	

	
	
	for(int y=0; y<maxn/size; y++){
		if(y == 0){
			for(int x=0; x<maxn; x++){
				printf("%.0f ",xlocal.beforeLine[x]);
			}
			printf("\n");
		}
		for(int x=0; x<maxn; x++){
			printf("%.0f ",xlocal.tab[x][y]);
		}
		printf("\n");
		if(y == maxn/size-1){
			for(int x=0; x<maxn; x++){
				printf("%.0f ",xlocal.afterLine[x]);
			}
			printf("\n");
		}
	}
	
	MPI_Finalize();
	
	return EXIT_SUCCESS;
}
