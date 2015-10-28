SHELL = /bin/bash
FLAGS = -Wall -Wextra -std=gnu99
LINKS = -lm
LAPLACE_SEQ_OBJ = tools/array2D.o
LAPLACE_OBJ = ${LAPLACE_SEQ_OBJ} tools/local_matrix.o
LAPLACE2D_OBJ = ${LAPLACE_SEQ_OBJ} tools/local_matrix2D.o tools/rank2D.o

NODE = `echo gemini{1..12} | sed 's/ /,/g'`
TASKS = 4

%.o: %.c
	mpicc -c ${FLAGS} $< -o $@

pi: pi.c
	mpicc ${FLAGS} -o pi.out $<
	mpirun -host ${NODE} -np ${TASKS} ./pi.out

seq_laplace: seq_laplace.c ${LAPLACE_SEQ_OBJ}
	gcc ${FLAGS} -o seq_laplace.out ${LAPLACE_SEQ_OBJ} $< ${LINKS}

grid: grid.c ${LAPLACE_OBJ}
	mpicc ${FLAGS} -o grid.out ${LAPLACE_OBJ} $<
	mpirun -host ${NODE} -np ${TASKS} ./grid.out

laplace: laplace.c ${LAPLACE_OBJ}
	mpicc ${FLAGS} -o laplace.out ${LAPLACE_OBJ} $< ${LINKS}
	mpirun -host ${NODE} -np ${TASKS} ./laplace.out

laplace2D: laplace2D.c ${LAPLACE2D_OBJ}
	mpicc ${FLAGS} -o laplace2D.out ${LAPLACE2D_OBJ} $< ${LINKS}
	mpirun -host ${NODE} -np ${TASKS} ./laplace2D.out

clean:
	rm -f **/*.o
	rm -f *.out
