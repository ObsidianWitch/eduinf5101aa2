#include <math.h>
#include "rank2D.h"

/**
 * Retrieves current rank position in a 2D space.
 */
Rank2D rank2D(int nprocs, int rank) {
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
int up(int nprocs, int rank) {
    return rank - (int) sqrt(nprocs);
}

/**
 * Retrives rank of the process handling the submatrix below the current one.
 */
int down(int nprocs, int rank) {
    return rank + (int) sqrt(nprocs);
}

/**
 * Retrives rank of the process handling the submatrix left to the current one.
 */
int left(int rank) {
    return rank - 1;
}

/**
 * Retrives rank of the process handling the submatrix right to the current one.
 */
int right(int rank) {
    return rank + 1;
}
