#include <stdio.h>
// #include <stdlib.h>
#include <mpi.h>
#include <gmp.h>

int main(int argc, char **argv) {
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
        mpz_t big_num;
        mpz_init(big_num);
        mpz_set_ui(big_num, 999999999);

        // print bignum
        gmp_printf("Number from GMP: %Zd\n", big_num);

        // cleanup
        mpz_clear(big_num);
    }

    MPI_Finalize();

}