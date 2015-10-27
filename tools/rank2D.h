#ifndef RANK2D
#define RANK2D

typedef struct Rank2D {
    int i;
    int j;
} Rank2D;

Rank2D rank2D(int nprocs, int rank);
int up(int nprocs, int rank);
int down(int nprocs, int rank);
int left(int rank);
int right(int rank);

#endif // RANK2D
