#NODE=echo gemini{1..12} | sed 's/ /,/g'
NODE=gemini1,gemini2,gemini3,gemini4,gemini5,gemini6,gemini7,gemini8,gemini9,gemini10,gemini11,gemini12
pi: pi.c
	@mpicc -Wall -Wextra -std=gnu99 -lm $<

grid: grid.c
	@mpicc -Wall -Wextra -std=gnu99 -lm $<
	@mpirun -host ${NODE} -np 12 ./a.out
	
run:
	mpirun -host ${NODE} -np 96 ./a.out
