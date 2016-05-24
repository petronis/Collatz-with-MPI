#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define false 0
#define true !false

#define MASTER 0
#define master() (rank == MASTER)

long collatz(long, long, int);

int main(int argc, char** argv)
{

    long m = atol(argv[1]), n = atol(argv[2]) + 1;
    long blockSize = atoi(argv[3]);
    int outputFlag = atoi(argv[4]);

    int rank = 0, np;
    int busy = 0;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &np);
    MPI_Status status;

    if (master()) // Only rank 0 will get to this
    {
           printf("Total number of procs: %d\r\n", np);
    }

    long maxIterations = 0;

    double start = MPI_Wtime();
    if (master()) // Only rank 0 will get to this
    {
        long iterations;
        long i = m, p;
        for (p = 1; p < np && i < n; i += blockSize, p += 1) // We divide our interval into block size and will send only that block
        {
            MPI_Send(&i, 1, MPI_LONG, p, 1, MPI_COMM_WORLD); // We send out the message through MPI
            busy++;
        }
        while (i < n)
        {
            MPI_Recv(&iterations, 1, MPI_LONG, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, &status); // We recieve block that was send from master
             if (outputFlag != false) // If output flag is not 0 we will get resoult how many iterations it took for every element
                 printf("%ld\r\n", iterations);
            MPI_Send(&i, 1, MPI_LONG, status.MPI_SOURCE, 1, MPI_COMM_WORLD);  // We send out message
            i += blockSize; // increase i by block size
        }
        for (p = 1; p <= busy; p++) 
        {
            MPI_Recv(&iterations, 1, MPI_LONG, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, &status); // We recieve the iteration count of every element
            if (outputFlag != false)
                 printf("%ld\r\n", iterations);
        }
        long endOfWork = -1;
        for (p = 1; p < np; p++)
        {
            MPI_Send(&endOfWork, 1, MPI_LONG, p, 1, MPI_COMM_WORLD); // We send out that the work has ended
        }
    }
    else // If its not master but a slave
    {
        while (true)
        {
            long start_i, end_i;
            MPI_Recv(&start_i, 1, MPI_LONG, MASTER, 1, MPI_COMM_WORLD, &status); // We recieve interval of block size
            if (start_i < 0) // If we get that the start of interval is less 0 we break
                break;
            end_i = start_i + blockSize;
            long job, longestJob = 0;
            long longestIt = 0, tempIt;
            for (job = start_i; job < end_i; job++) // For every interval of block size we do collatz
            {
                tempIt = collatz(job,0,outputFlag);
                if (tempIt > longestIt) // We count the longes iteration of interval (this is just to be sure if algorithm is working okay)
                {
                    longestIt = tempIt;
                    longestJob = job;
                }
            }
            MPI_Send(&longestIt, 1, MPI_LONG, MASTER, 1, MPI_COMM_WORLD); // We send out the longes iteration of interval to master that he could print out if its needed
        }
    }
    double end = MPI_Wtime(); // We get the end time

    if (master())
    {
        printf("Job is done. It took this much time: %.4f\r\n",(end - start));
    }
    MPI_Finalize();
    return 0;
}

long collatz(long n, long iterations, int outputFlag){ // Recursive algorithm for collatz problem
        if (outputFlag != false){ // We print out every n (number of the interval) before doing collatz algorithm (this is just to check if everything is okay)
                printf("Collatz %d\n",n);
        }
        if(n == 1){
                return iterations;
        } else if(n%2 == 0){
                n = n/2;
                iterations++;
                iterations = collatz(n, iterations, outputFlag);
                return iterations;
        } else{
                n =  n*3+1;
                iterations++;
                iterations = collatz(n, iterations, outputFlag);
                return iterations;
        }
}
