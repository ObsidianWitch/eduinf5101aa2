#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "mpi.h"

double arctan(double a, double b, double n) {
	double sum = 0;
	for (double x = a ; x < b ; x += 1./n) {
		sum += (1. / (1. + x*x)) * 1./n;
	}
	return sum;
}

void writeResult(double reduceBuffer) {
	FILE* f = fopen("pi_result.out", "w");
    if (f == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }
	
	fprintf(f, "%.42f\n", reduceBuffer);
	fprintf(f, "%.42f\n", M_PI);
	
	fclose(f);
}

int main(int argc, char** argv) {
	int size, rank;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	
	if (argc < 2) {
		printf(
			"Missing arguments\n"
			"Usage : %s INTERALS\n",
			argv[0]
		);
		return EXIT_FAILURE;
	}
	double intervals = atof(argv[1]);
	
    double startTime;
    if (rank == 0) { startTime = MPI_Wtime(); }
	
	double a = (double) rank / size;
	double b = (double) (rank + 1) / size;
	double localResult = 4 * arctan(a, b, intervals);
	
	double reduceBuffer;
	MPI_Reduce(
		&localResult, &reduceBuffer, 1,
		MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD
	);
	
	if (rank == 0) {
		writeResult(reduceBuffer);
		printf("computation time: %f s\n", MPI_Wtime() - startTime);
	}
	
	MPI_Finalize();
	
	return EXIT_SUCCESS;
}
