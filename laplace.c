#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "mpi.h"
#include "tools/local_matrix.h"

const int N_MATRIX = 12;
const float DELTA = 1.0e-6;

void computeLaplaceEquation(LocalMatrix* localMatrix, int nprocs, int rank) {
    int iStart = 1;
    int iEnd = localMatrix->totalLines;
    
    if (rank == 0) { iStart++; }
    if (rank == nprocs -1) { iEnd--; }
    
    double err = 1.0;
    while (err > DELTA) {
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

    LocalMatrix localMatrix = createLocalMatrix(nprocs, N_MATRIX, rank);

    MPI_Barrier(MPI_COMM_WORLD);

    computeLaplaceEquation(&localMatrix, nprocs, rank);

    writeFullMatrix(&localMatrix, nprocs, rank, false);

    destructLocalMatrix(&localMatrix);

    MPI_Finalize();

    return EXIT_SUCCESS;
}
