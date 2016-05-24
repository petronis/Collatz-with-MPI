# Collatz-with-MPI
MPI program for Collatz problem with C 
Works on cluster
To compile use: mpicc -o Ketvirta Ketvirta.c
To run use: mpirun (-np x) Ketvirta n m z y (np flag is to use certain amout of process) ( [n;m] - interval int type, 
z - block size to which the interval will be divided, y is outputFlad if y = 0 no output only the time that the algorithm used, 
anything else will give every output)
