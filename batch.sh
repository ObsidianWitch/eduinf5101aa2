#!/bin/bash
NODES=`echo gemini{1..12} | sed 's/ /,/g'`

# fixed parameters
DELTA=1.0e-6

PROGRESS=0
TOTAL_PROGRESS=64
function printProgress() {
    ((PROGRESS+=1))
    echo "progress: ${PROGRESS}/${TOTAL_PROGRESS}"
}

# $1: ntasks
# $2: intervals
function run_pi() {
    echo "pi $1 $2"
    echo "pi $1 $2" >> results.out
    mpirun -host ${NODES} -np $1 ./pi.out $2 >> results.out
    echo "" >> results.out
}

# $1: ntasks
# $2: nmatrix
function run_laplace() {
    echo "laplace $1 $2"
    echo "laplace $1 $2" >> results.out
    mpirun -host ${NODES} -np $1 ./laplace.out $2 ${DELTA} >> results.out
    echo "" >> results.out
}

# $1: ntasks
# $2: nmatrix
function run_laplace2D() {
    echo "laplace2D $1 $2"
    echo "laplace2D $1 $2" >> results.out
    mpirun -host ${NODES} -np $1 ./laplace2D.out $2 ${DELTA} >> results.out
    echo "" >> results.out
}

function batch() {
    local ntasks_list="1 4 9 36"
    
    for ntasks in $ntasks_list; do
        local intervals_list="1.0e7 1.0e8 1.0e9 1.0e10"
        for intervals in $intervals_list; do
            printProgress
            run_pi $ntasks $intervals
            echo ""
        done
        
        local nmatrix_list="36 72 108 144 180 216"
        for nmatrix in $nmatrix_list; do
            printProgress
            run_laplace $ntasks $nmatrix
            echo ""
            
            printProgress
            run_laplace2D $ntasks $nmatrix
            echo ""
        done
    done
}

make pi laplace laplace2D

rm results.out
batch
