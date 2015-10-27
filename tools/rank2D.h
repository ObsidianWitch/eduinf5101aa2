#ifndef RANK2D
#define RANK2D

typedef struct Rank2D {
    int rank;
    int i;
    int j;
} Rank2D;

Rank2D createRank2D(int nprocs, int rank);
int upRank(int nprocs2D, int rank);
int downRank(int nprocs2D, int rank);
int leftRank(int rank);
int rightRank(int rank);

#endif // RANK2D
