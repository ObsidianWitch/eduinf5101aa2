#include <stdlib.h>
#include <stdio.h>
#include "mpi.h"
#include "tools/local_matrix.h"

const int N_MATRIX = 12;

int main(int argc, char** argv) {
    int nprocs, rank;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

    LocalMatrix localMatrix = createLocalMatrix(nprocs, N_MATRIX, rank);

    MPI_Barrier(MPI_COMM_WORLD);

    WriteFullMatrix(&localMatrix, nprocs, rank);

    destructLocalMatrix(&localMatrix);

    MPI_Finalize();

    return EXIT_SUCCESS;
}
