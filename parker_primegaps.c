#include <stdio.h>
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

    mpz_set_ui(max, 100);
    mpz_set_ui(previous, 1);
    mpz_nextprime(num, previous);

    // generate the array of prime numbers
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