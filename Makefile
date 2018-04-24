CC=gcc

all: monteCarloPi_Seq  monteCarloPi_omp copyOfmonteCarloPi_Seq

monteCarloPi_Seq: monteCarloPi_Seq.c
	${CC} -o monteCarloPi_Seq monteCarloPi_Seq.c

monteCarloPi_omp: monteCarloPi_omp.c
	${CC} -o monteCarloPi_omp monteCarloPi_omp.c -fopenmp

copyOfmonteCarloPi_Seq: copyOfmonteCarloPi_Seq.c
	${CC} -o copyOfmonteCarloPi_Seq copyOfmonteCarloPi_Seq.c
