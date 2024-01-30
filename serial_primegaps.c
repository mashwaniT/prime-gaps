#include <stdio.h>
#include <time.h>
// #include <stdlib.h>
// #include <mpi.h>
#include <gmp.h>

int main(int argc, char **argv) {

    mpz_t num, previous, max, gap, lprime, rprime, difference;
    mpz_init(num);
    mpz_init(previous);
    mpz_init(max);
    mpz_init(lprime);
    mpz_init(rprime);
    mpz_init(difference);
    mpz_init(gap);

    clock_t start_time, end_time;
    double cpu_time_used;

    // Record the starting time
    start_time = clock();

    mpz_set_ui(max, 1000000000);
    mpz_set_ui(previous, 1);
    mpz_nextprime(num, previous);

    while (mpz_cmp(num, max) < 0)
    {

        mpz_sub(difference, num, previous);

        if (mpz_cmp(difference, gap) > 0 )
        {
            mpz_set(gap, difference);
            mpz_set(lprime, previous);
            mpz_set(rprime, num);
        }

        mpz_set(previous, num);
        
        // find the next prime
        mpz_nextprime(num, num);

    }

    // Record the ending time
    end_time = clock();

    // Calculate the CPU time used
    cpu_time_used = ((double) (end_time - start_time)) / CLOCKS_PER_SEC;

    // Print the time taken
    printf("Time taken: %f seconds\n", cpu_time_used);

    gmp_printf("lprime: %Zd rprime: %Zd gap: %Zd\n", lprime, rprime, gap);
    
    // // initialize the MPI env
    // MPI_Init(NULL, NULL);

    // // get number of processes
    // int world_size;
    // MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // // get rank of process
    // int world_rank;
    // MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    // // print hello world from each process
    // printf("Hello world from processor with rank %d out of %d processors\n", world_rank, world_size);

    // // master process task
    // if (world_rank == 0) {
        
    // } else {
        
    // }

    // MPI_Finalize();

}