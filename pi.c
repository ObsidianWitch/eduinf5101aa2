#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "mpi.h"

const double INTERVALS = 1.0e8;

double arctan(double a, double b, double n) {
	double sum = 0;
	for (double x = a ; x < b ; x += 1./n) {
		sum += (1. / (1. + x*x)) * 1./n;
	}
	return sum;
}

int main(int argc, char** argv) {
	int size, rank;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank( MPI_COMM_WORLD, &rank );
	MPI_Comm_size( MPI_COMM_WORLD, &size );
	
	double a = (double) rank / size;
	double b = (double) (rank + 1) / size;
	double localResult = 4 * arctan(a, b, INTERVALS);
	
	double reduceBuffer;
	MPI_Reduce(
		&localResult, &reduceBuffer, 1,
		MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD
	);
	
	if (rank == 0) {
		printf("%.42f\n", reduceBuffer);
		printf("%.42f\n", M_PI);
	}
	
	MPI_Finalize();
	
	return EXIT_SUCCESS;
}
