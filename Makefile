#NODE=echo gemini{1..12} | sed 's/ /,/g'
NODE=pc5103a,pc5103b,pc5103c,pc5103d,pc5103e,pc5103f,pc5103g,pc5103h,pc5103i,pc5103j,pc5103k,pc5103l
pi: pi.c
	@mpicc -Wall -Wextra -std=gnu99 -lm $<

grid: grid.c
	@mpicc -Wall -Wextra -std=gnu99 -lm $<
	@mpirun -host ${NODE} -np 12 ./a.out
	
run:
	mpirun -host ${NODE} -np 96 ./a.out
