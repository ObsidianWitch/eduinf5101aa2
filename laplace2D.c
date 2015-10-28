#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "mpi.h"
#include "tools/local_matrix2D.h"
#include "tools/rank2D.h"

const int N_MATRIX = 12;
const float DELTA = 1.0e-6;

void computeLaplaceEquation(LocalMatrix* localMatrix, int nprocs, Rank2D rank2D) {
    int nprocs2D = sqrt(nprocs);
    
    int iStart = 1;
    if (rank2D.i == 0) { iStart++; }
    
    int iEnd = localMatrix->totalSize;
    if (rank2D.i == nprocs2D - 1) { iEnd--; }
    
    int jStart = 1;
    if (rank2D.j == 0) { jStart++; }
    
    int jEnd = localMatrix->totalSize;
    if (rank2D.j == nprocs2D - 1) { jEnd--; }
    
    double err = 1.0;
    while (err > DELTA) {
        err = 0;
        for (int i = iStart ; i < iEnd - 1 ; i++) {
            for (int j = jStart ; j < jEnd - 1 ; j++) {
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
        
        updateBoundaries(localMatrix, nprocs, rank2D);
    }
}

int main(int argc, char** argv) {
    int nprocs, rank;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    
    Rank2D rank2D = createRank2D(nprocs, rank);
    LocalMatrix localMatrix = createLocalMatrix(nprocs, N_MATRIX, rank2D);

    MPI_Barrier(MPI_COMM_WORLD);
    
    computeLaplaceEquation(&localMatrix, nprocs, rank2D);

    writeFullMatrix(&localMatrix, nprocs, rank2D, false);

    destructLocalMatrix(&localMatrix);

    MPI_Finalize();

    return EXIT_SUCCESS;
}
