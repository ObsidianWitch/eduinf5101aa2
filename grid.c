#include <stdlib.h>
#include <stdio.h>
#include "mpi.h"
#include "tools/local_matrix.h"

int main(int argc, char** argv) {
    int nprocs, rank;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    
    if (argc < 2) {
        printf(
            "Missing arguments\n"
            "Usage : %s N_MATRIX\n",
            argv[0]
        );
        return EXIT_FAILURE;
    }
    int nmatrix = atoi(argv[1]);

    LocalMatrix localMatrix = createLocalMatrix(nprocs, nmatrix, rank);

    MPI_Barrier(MPI_COMM_WORLD);

    writeFullMatrix(&localMatrix, nprocs, rank, true);

    destructLocalMatrix(&localMatrix);

    MPI_Finalize();

    return EXIT_SUCCESS;
}
