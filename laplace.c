#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "mpi.h"
#include "tools/local_matrix.h"

void computeLaplaceEquation(
    LocalMatrix* localMatrix, int nprocs, int rank, int delta
) {
    int iStart = 1;
    int iEnd = localMatrix->totalLines;
    
    if (rank == 0) { iStart++; }
    if (rank == nprocs -1) { iEnd--; }
    
    double err = 1.0;
    while (err > delta) {
        err = 0;
        for (int i = iStart ; i < iEnd - 1 ; i++) {
            for (int j = 1 ; j < localMatrix->cols - 1 ; j++) {
                double newValue = 0.25 * (
                    get(localMatrix, i + 1, j) +
                    get(localMatrix, i - 1, j) +
                    get(localMatrix, i, j + 1) +
                    get(localMatrix, i, j - 1)
                );
                
                err += newValue - get(localMatrix, i, j);
                err *= err;
                set(localMatrix, i, j, newValue);
            }
        }
        
        double tmpErr;
        MPI_Allreduce(&err, &tmpErr, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
        err = sqrt(tmpErr);
        
        updateBoundaries(localMatrix, nprocs, rank);
    }
}

int main(int argc, char** argv) {
    int nprocs, rank;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    
    if (argc < 3) {
        printf(
            "Missing arguments\n"
            "Usage : %s N_MATRIX DELTA\n",
            argv[0]
        );
        return EXIT_FAILURE;
    }
    int nmatrix = atoi(argv[1]);
    float delta = atof(argv[2]);

    LocalMatrix localMatrix = createLocalMatrix(nprocs, nmatrix, rank);

    MPI_Barrier(MPI_COMM_WORLD);

    computeLaplaceEquation(&localMatrix, nprocs, rank, delta);

    writeFullMatrix(&localMatrix, nprocs, rank, false);

    destructLocalMatrix(&localMatrix);

    MPI_Finalize();

    return EXIT_SUCCESS;
}
