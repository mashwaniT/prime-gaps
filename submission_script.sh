#!/bin/bash
#SBATCH --nodes=4
#SBATCH --ntasks-per-node=8
#SBATCH --time=0-00:05
#SBATCH --job-name mpi_job_primegaps
#SBATCH --output=output/mpi_output_%j.txt

cd $SLURM_SUBMIT_DIR

module load gcc
module load openmpi
module load gmp

mpirun ./primegaps      # mpirun or mpiexec also work