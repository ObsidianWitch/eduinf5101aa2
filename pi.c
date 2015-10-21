#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "mpi.h"

double arctan(double a, double b, double n){
	double sum = 0;
	for(double x=a; x<b; x+=1./n){
		sum += (1./(1.+x*x)) * 1./n;
	}
	return sum;
}

int main(int argc, char** argv) {
	int size, rank;
	double localResult;
	double reduceBuffer;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank( MPI_COMM_WORLD, &rank );
	MPI_Comm_size( MPI_COMM_WORLD, &size );
	double drank = (double)rank;
	double dsize = (double)size;
	double a = drank/dsize;
	double b = (drank+1)/dsize;
	
	localResult = 4*arctan(a,b,10000000000000.);
	
/*	MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);*/
	MPI_Reduce(&localResult, &reduceBuffer, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
/*	printf("{%d} PI[%.2f,%.2f] = %f\n",rank, a, b, 4*arctan(a,b,10));*/
/*	printf("%f\n",);*/
	
	
	MPI_Finalize();
	if(rank == 0){
		printf("%.42f\n", reduceBuffer);
		printf("%.42f\n", M_PI);
	}
	
	return EXIT_SUCCESS;
}
