// include statements
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>
#include <gmp.h>

// Using max fixed size instead of dynamic allocation because I kept running into buffer problems
#define MAX_STRING_LENGTH 1024 

int main(int argc, char** argv) {

    // Global variables to be used by each process
    int world_rank, world_size;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // Range for each process
    mpz_t range_start, range_end, global_range;

    // Initialize and set the global range to 1 billion
    mpz_init_set_ui(global_range, 1000000000); 

    mpz_t current_prime, next_prime, gap, max_gap, last_prime_in_range;
    mpz_t prime_before_gap, prime_after_gap;

    // Initialize all variables
    mpz_init(current_prime);
    mpz_init(next_prime);
    mpz_init(gap);
    mpz_init(max_gap);
    mpz_init(last_prime_in_range);
    mpz_init(prime_before_gap);
    mpz_init(prime_after_gap);
    mpz_set_ui(max_gap, 0);

    // ****************************************************************

    // Calculate the range per process
    unsigned long long int range_per_process = mpz_get_ui(global_range) / world_size;

    // Set the start and end range for the current process
    mpz_init_set_ui(range_start, world_rank * range_per_process + 1);
    mpz_init_set_ui(range_end, (world_rank + 1) * range_per_process);

    // Adjusted for last process - let's it cover entire range
    if (world_rank == world_size - 1) {
        mpz_set(range_end, global_range);
    }

    // Synchronize all processes
    MPI_Barrier(MPI_COMM_WORLD); 

    // Get the starting time
    double start_time = MPI_Wtime();

    // Get the next prime starting from range_start
    mpz_nextprime(current_prime, range_start);

    while (mpz_cmp(current_prime, range_end) <= 0) {
        
        // Get next prime and calculate gap
        mpz_nextprime(next_prime, current_prime);

        // Calculate the gap between the current and next prime
        mpz_sub(gap, next_prime, current_prime);

        // store if largest gap
        if (mpz_cmp(gap, max_gap) > 0) {
            mpz_set(max_gap, gap);
            mpz_set(prime_before_gap, current_prime);
            mpz_set(prime_after_gap, next_prime);
        }

        // Move to the next prime
        mpz_set(current_prime, next_prime);
    }

    // Set the last prime in the range
    mpz_set(last_prime_in_range, current_prime);

    // Synchronize all processes
    MPI_Barrier(MPI_COMM_WORLD);

    // Get the ending time
    double end_time = MPI_Wtime();

    // Calculate the elapsed time
    double elapsed_time = end_time - start_time;

    // Get the resolution of MPI_Wtime
    double tick = MPI_Wtick();

    // ****************************************************************

    // Prepare strings for communication
    char max_gap_str[MAX_STRING_LENGTH], prime_before_str[MAX_STRING_LENGTH], prime_after_str[MAX_STRING_LENGTH];
    
    // Initialize strings
    memset(max_gap_str, 0, MAX_STRING_LENGTH);
    memset(prime_before_str, 0, MAX_STRING_LENGTH);
    memset(prime_after_str, 0, MAX_STRING_LENGTH);
    mpz_get_str(max_gap_str, 10, max_gap);

    // Convert prime_before_gap and prime_after_gap to string
    mpz_get_str(prime_before_str, 10, prime_before_gap);
    mpz_get_str(prime_after_str, 10, prime_after_gap);

    // Allocate memory for string receive in process 0
    char (*all_max_gap_str)[MAX_STRING_LENGTH] = NULL;
    char (*all_prime_before_str)[MAX_STRING_LENGTH] = NULL;
    char (*all_prime_after_str)[MAX_STRING_LENGTH] = NULL;
    
    if (world_rank == 0) {

        // all_max_gap_str = malloc(world_size * MAX_STRING_LENGTH * sizeof(char));
        // all_prime_before_str = malloc(world_size * MAX_STRING_LENGTH * sizeof(char));
        // all_prime_after_str = malloc(world_size * MAX_STRING_LENGTH * sizeof(char));

        // This lets it compile with mpi c++ above only compiles with mpic
        all_max_gap_str = (char (*)[MAX_STRING_LENGTH])malloc(world_size * MAX_STRING_LENGTH * sizeof(char));
        all_prime_before_str = (char (*)[MAX_STRING_LENGTH])malloc(world_size * MAX_STRING_LENGTH * sizeof(char));
        all_prime_after_str = (char (*)[MAX_STRING_LENGTH])malloc(world_size * MAX_STRING_LENGTH * sizeof(char));
    }

    // Gather fixed size strings
    MPI_Gather(max_gap_str, MAX_STRING_LENGTH, MPI_CHAR, all_max_gap_str, MAX_STRING_LENGTH, MPI_CHAR, 0, MPI_COMM_WORLD);
    MPI_Gather(prime_before_str, MAX_STRING_LENGTH, MPI_CHAR, all_prime_before_str, MAX_STRING_LENGTH, MPI_CHAR, 0, MPI_COMM_WORLD);
    MPI_Gather(prime_after_str, MAX_STRING_LENGTH, MPI_CHAR, all_prime_after_str, MAX_STRING_LENGTH, MPI_CHAR, 0, MPI_COMM_WORLD);

    if (world_rank == 0) {
        mpz_t global_max_gap, global_prime_before_gap, global_prime_after_gap;
        
        //  Initialize global values
        mpz_init(global_max_gap);
        mpz_init(global_prime_before_gap);
        mpz_init(global_prime_after_gap);
        mpz_set_ui(global_max_gap, 0);

        // Process received strings
        for (int i = 0; i < world_size; ++i) {
            mpz_t temp_max_gap;
            mpz_init(temp_max_gap);
            mpz_set_str(temp_max_gap, all_max_gap_str[i], 10);

            // gmp_printf("temp_max_gap: %Zd\n", temp_max_gap);
            // gmp_printf("global_max_gap: %Zd\n", global_max_gap);

            if (mpz_cmp(temp_max_gap, global_max_gap) > 0) {
                mpz_set(global_max_gap, temp_max_gap);
                mpz_set_str(global_prime_before_gap, all_prime_before_str[i], 10);
                mpz_set_str(global_prime_after_gap, all_prime_after_str[i], 10);
            }

            // Clear temporary maximum gap
            mpz_clear(temp_max_gap);
        }

        // gmp_printf("Largest gap: %Zd, between primes %Zd and %Zd\n", max_gap, prime_before_gap, prime_after_gap);
        gmp_printf("Largest gap: %Zd, between primes %Zd and %Zd\n", global_max_gap, global_prime_before_gap, global_prime_after_gap);
        printf("Total processes: %d\n", world_size);
        printf("Total computation time: %e seconds\n", elapsed_time);
        printf("Average computation time per process: %e seconds\n", elapsed_time / world_size);
        printf("Resolution of MPI_Wtime: %e seconds\n", tick);

        // Cleanup
        mpz_clear(global_max_gap);
        mpz_clear(global_prime_before_gap);
        mpz_clear(global_prime_after_gap);
        free(all_max_gap_str);
        free(all_prime_before_str);
        free(all_prime_after_str);
    }

    // Cleanup
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