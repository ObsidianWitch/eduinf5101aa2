#!/bin/bash
NODES = `echo gemini{1..12} | sed 's/ /,/g'`
TASKS=4

INTERVALS=1.0e8
N_MATRIX=12
N_SUBMATRICES=${TASKS}
DELTA=1.0e-6

if [ "$#" -lt 1 ]; then
    echo "Missing arguments"
    echo "Usage: $0 program"
    exit 1
fi

if [ "$1" == "pi" ]; then
    make pi \
    && mpirun -host ${NODES} -np ${TASKS} ./pi.out ${INTERVALS} \
    && more pi_result.out
elif [ "$1" == "seq_laplace" ]; then
    make seq_laplace \
    && ./seq_laplace.out ${N_MATRIX} ${N_SUBMATRICES} ${DELTA} \
    && more matrix.out
elif [ "$1" == "grid" ]; then
    make grid \
    && mpirun -host ${NODES} -np ${TASKS} ./grid.out ${N_MATRIX} \
    && more matrix.out
elif [ "$1" == "laplace" ]; then
    make laplace \
    && mpirun -host ${NODES} -np ${TASKS} ./laplace.out ${N_MATRIX} ${DELTA} \
    && more matrix.out
elif [ "$1" == "laplace2D" ]; then
    make laplace2D \
    && mpirun -host ${NODES} -np ${TASKS} ./laplace2D.out ${N_MATRIX} ${DELTA} \
    && more matrix.out
else
    echo "$1 does not exist"
    exit 1
fi

exit 0
