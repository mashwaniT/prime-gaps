# Prime Gaps MPI Program

## Introduction
This program is designed to calculate the largest gap between consecutive prime numbers up to one billion using the MPI (Message Passing Interface) for parallel computation and the GMP (GNU Multiple Precision Arithmetic) library for precise arithmetic operations. It showcases the use of parallel computing resources to solve computationally intensive problems efficiently.

## Features
- Utilizes MPI for distributed computing.
- Employs GMP for handling large numbers and precise calculations.
- Includes performance benchmarks for different processor counts.

## Prerequisites
Before you begin, ensure you have met the following requirements:
- You have a `Linux/MacOS` machine.
- You have installed `MPI` and `GMP` libraries.

## Setup and Installation
1. **Clone the repository:**
    https://github.com/mashwaniT/prime-gaps.git
2. **Navigate to the project directory:**


## Building the Program
1. **Compile the program using the provided Makefile:**
```bash
make
```
This command will compile the source code and generate an executable named `primegaps`.

## Running the Program
1. **Execute the program with MPI:**
```bash
make run
```
This command will run the program using `mpiexec` with the number of processes specified in the `Makefile`. You can adjust the number of processes by modifying the `NPROCS` variable in the `Makefile`.

## Contributing to the Project
Contributions to the project are welcome! Here's how you can contribute:
1. **Fork the repository.**
2. **Create a new branch:** `git checkout -b <branch_name>`.
3. **Make your changes and commit them:** `git commit -m '<commit_message>'`.
4. **Push to the original branch:** `git push origin <project_name>/<location>`.
5. **Create the pull request.**

Alternatively, see the GitHub documentation on [creating a pull request](https://help.github.com/articles/creating-a-pull-request/).
