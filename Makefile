SHELL = /bin/bash
FLAGS = -Wall -Wextra -std=gnu99
LINKS = -lm
NODE = `echo gemini{1..12} | sed 's/ /,/g'`
TASKS = 4
INCLUDES = tools/array2D.h tools/array2D.c \
           tools/local_matrix.h tools/local_matrix.c

pi: pi.c
	mpicc ${FLAGS} -o pi.out $<
	mpirun -host ${NODE} -np ${TASKS} ./pi.out

grid: grid.c
	mpicc ${FLAGS} -o grid.out ${INCLUDES} $<
	mpirun -host ${NODE} -np ${TASKS} ./grid.out

laplace: laplace.c
	mpicc ${FLAGS} -o laplace.out ${INCLUDES} $< ${LINKS}
	mpirun -host ${NODE} -np ${TASKS} ./laplace.out

seq_laplace: seq_laplace.c
	gcc ${FLAGS} -o seq_laplace.out tools/array2D.c $< ${LINKS}

clean:
	rm -f *.out
