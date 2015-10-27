#include <math.h>
#include "rank2D.h"

/**
 * Retrieves current rank position in a 2D space.
 */
Rank2D createRank2D(int nprocs, int rank) {
    Rank2D rank2D = {
        .rank = rank,
        .i    = rank / (int) sqrt(nprocs),
        .j    = rank % (int) sqrt(nprocs)
    };
    
    return rank2D;
}

/**
 * Retrives rank of the process handling the submatrix above the current one.
 */
int upRank(int nprocs2D, int rank) {
    return rank - nprocs2D;
}

/**
 * Retrives rank of the process handling the submatrix below the current one.
 */
int downRank(int nprocs2D, int rank) {
    return rank + nprocs2D;
}

/**
 * Retrives rank of the process handling the submatrix left to the current one.
 */
int leftRank(int rank) {
    return rank - 1;
}

/**
 * Retrives rank of the process handling the submatrix right to the current one.
 */
int rightRank(int rank) {
    return rank + 1;
}
