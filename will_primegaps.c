#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>
#include <gmp.h>

int main(int argc, char** argv) {

    int world_rank, world_size;

    MPI_Init(&argc, &argv);

    // Find out process rank
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    // Find out number of processes
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // range for each process
    mpz_t range_start, range_end, global_range;
    mpz_init_set_ui(global_range, 1000000); // 1 billion
    unsigned long long int range_per_process = mpz_get_ui(global_range) / world_size;

    mpz_init_set_ui(range_start, world_rank * range_per_process + 1);
    mpz_init_set_ui(range_end, (world_rank + 1) * range_per_process);

    // adjusted for last process - let's it cover entire range
    if (world_rank == world_size - 1) {
        mpz_set(range_end, global_range);
    }

    mpz_t current_prime, next_prime, gap, max_gap, last_prime_in_range;
    mpz_t prime_before_gap, prime_after_gap;

    mpz_init(current_prime);
    mpz_init(next_prime);
    mpz_init(gap);
    mpz_init(max_gap);
    mpz_init(last_prime_in_range);
    mpz_init(prime_before_gap);
    mpz_init(prime_after_gap);
    mpz_set_ui(max_gap, 0);

    MPI_Barrier(MPI_COMM_WORLD); // sync point
    double start_time = MPI_Wtime();

    gmp_printf("Processor Rank: %d Range Start: %Zd Range End: %Zd\n", world_rank, range_start, range_end);

    mpz_nextprime(current_prime, range_start);

    while (mpz_cmp(current_prime, range_end) <= 0) {

        // get next prime and calculate gap
        mpz_nextprime(next_prime, current_prime);
        mpz_sub(gap, next_prime, current_prime);

        // store if largest gap
        if (mpz_cmp(gap, max_gap) > 0) {
            mpz_set(max_gap, gap);
            mpz_set(prime_before_gap, current_prime);
            mpz_set(prime_after_gap, next_prime);
        }

        mpz_set(current_prime, next_prime);
    }

    gmp_printf("Processor Rank: %d Largest Gap: %Zd Left Prime: %Zd Right Prime: %Zd\n", world_rank, max_gap, prime_before_gap, prime_after_gap);
    

    mpz_set(last_prime_in_range, current_prime);

    MPI_Barrier(MPI_COMM_WORLD); // sync point
    double end_time = MPI_Wtime();

    double elapsed_time = end_time - start_time;
    double tick = MPI_Wtick();

    // convert strings for communication
    char max_gap_str[1024], prime_before_str[1024], prime_after_str[1024];
    mpz_get_str(max_gap_str, 10, max_gap);
    mpz_get_str(prime_before_str, 10, prime_before_gap);
    mpz_get_str(prime_after_str, 10, prime_after_gap);

    int local_lengths[3] = {strlen(max_gap_str) + 1, strlen(prime_before_str) + 1, strlen(prime_after_str) + 1};
    int *global_lengths = NULL;
    if (world_rank == 0) {
        global_lengths = (int*)malloc(world_size * 3 * sizeof(int)); // memory for MPI gather
    }

    char* all_max_gap_str = NULL;
    char* all_prime_before_str = NULL;
    char* all_prime_after_str = NULL;

    MPI_Gather(local_lengths, 3, MPI_INT, global_lengths, 3, MPI_INT, 0, MPI_COMM_WORLD);

    if (world_rank == 0) {
        // get length and allocate memory for gathered strings
        int total_max_gap_length = 0, total_prime_before_length = 0, total_prime_after_length = 0;
        for (int i = 0; i < world_size; ++i) {
            total_max_gap_length += global_lengths[i * 3];
            total_prime_before_length += global_lengths[i * 3 + 1];
            total_prime_after_length += global_lengths[i * 3 + 2];
        }
        all_max_gap_str = (char*)malloc(total_max_gap_length * sizeof(char));
        all_prime_before_str = (char*)malloc(total_prime_before_length * sizeof(char));
        all_prime_after_str = (char*)malloc(total_prime_after_length * sizeof(char));

        // store for global max gap
        mpz_t global_max_gap, global_prime_before_gap, global_prime_after_gap;
        mpz_init(global_max_gap);
        mpz_init(global_prime_before_gap);
        mpz_init(global_prime_after_gap);

        // process all data
        int offset = 0;
        for (int i = 0; i < world_size; ++i) {
            // get max gap. string in comms so convert back to mpz_t 
            char* temp_max_gap_str = (char*)malloc(global_lengths[i * 3] * sizeof(char));
            strncpy(temp_max_gap_str, all_max_gap_str + offset, global_lengths[i * 3]);
            printf("Received max gap string from process %d: %s\n", i, temp_max_gap_str);
            offset += global_lengths[i * 3];
            mpz_t temp_max_gap;
            mpz_init(temp_max_gap);
            mpz_set_str(temp_max_gap, temp_max_gap_str, 10);
            free(temp_max_gap_str);

            gmp_printf("temp_max_gap: %Zd\n", temp_max_gap);
            gmp_printf("global_max_gap: %Zd\n", global_max_gap);

            // find global maximum gap
            if (mpz_cmp(temp_max_gap, global_max_gap) > 0) {
                mpz_set(global_max_gap, temp_max_gap);

                char* temp_prime_before_str = (char*)malloc(global_lengths[i * 3 + 1] * sizeof(char));
                strncpy(temp_prime_before_str, all_prime_before_str + offset, global_lengths[i * 3 + 1]);
                mpz_set_str(global_prime_before_gap, temp_prime_before_str, 10);
                free(temp_prime_before_str);

                char* temp_prime_after_str = (char*)malloc(global_lengths[i * 3 + 2] * sizeof(char));
                strncpy(temp_prime_after_str, all_prime_after_str + offset, global_lengths[i * 3 + 2]);
                mpz_set_str(global_prime_after_gap, temp_prime_after_str, 10);
                free(temp_prime_after_str);
            }
            mpz_clear(temp_max_gap);
        }

        gmp_printf("Largest gap: %Zd, between primes %Zd and %Zd\n", max_gap, prime_before_gap, prime_after_gap);
        gmp_printf("Largest gap: %Zd, between primes %Zd and %Zd\n", global_max_gap, global_prime_before_gap, global_prime_after_gap);
        printf("Total processes: %d\n", world_size);
        printf("Total computation time: %e seconds\n", elapsed_time);
        printf("Average computation time per process: %e seconds\n", elapsed_time / world_size);
        printf("Resolution of MPI_Wtime: %e seconds\n", tick);

        // cleanup
        mpz_clear(global_max_gap);
        mpz_clear(global_prime_before_gap);
        mpz_clear(global_prime_after_gap);
        free(all_max_gap_str);
        free(all_prime_before_str);
        free(all_prime_after_str);
    }

     // cleanup
    mpz_clear(range_start);
    mpz_clear(range_end);
    mpz_clear(global_range);
    mpz_clear(current_prime);
    mpz_clear(next_prime);
    mpz_clear(gap);
    mpz_clear(max_gap);
    mpz_clear(last_prime_in_range);
    mpz_clear(prime_before_gap);
    mpz_clear(prime_after_gap);

    MPI_Finalize();
    return 0;
}