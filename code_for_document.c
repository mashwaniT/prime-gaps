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