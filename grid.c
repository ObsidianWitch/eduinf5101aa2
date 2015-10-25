#include <stdlib.h>
#include <stdio.h>
#include "mpi.h"
#include "tools/matrix.h"

const int N_MATRIX = 12;

int main(int argc, char** argv) {
    int nprocs, rank;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

    struct LocalMatrix localMatrix = createLocalMatrix(nprocs, N_MATRIX);
    localInitialization(&localMatrix, nprocs, N_MATRIX, rank);
    remoteInitialization(&localMatrix, nprocs, N_MATRIX, rank);

    MPI_Barrier(MPI_COMM_WORLD);

    printf("rank: %d\n", rank);
    display(&localMatrix, nprocs, N_MATRIX);
    printf("\n");

    destructLocalMatrix(&localMatrix);

    MPI_Finalize();

    return EXIT_SUCCESS;
}
