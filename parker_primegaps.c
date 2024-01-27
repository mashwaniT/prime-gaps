#include <stdio.h>
// #include <stdlib.h>
#include <mpi.h>
#include <gmp.h>

int main(int argc, char **argv) {

    mpz_t num;
    mpz_inti(num);

    // generate the array of prime numbers
    while (/* condition */)
    {
        // find the next prime
        mpz_nextprime (num, num);

        // Add the new prime number to the array
    }
    
    // initialize the MPI env
    MPI_Init(NULL, NULL);

    // get number of processes
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // get rank of process
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    // print hello world from each process
    printf("Hello world from processor with rank %d out of %d processors\n", world_rank, world_size);

    // master process task
    if (world_rank == 0) {
        
    } else {
        
    }

    MPI_Finalize();

}