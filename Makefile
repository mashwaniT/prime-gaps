# Define the compiler
CC=mpicc

# Define any compile-time flags
CFLAGS=-Wall -O3
# Include path for GMP
INCLUDES=-I/opt/homebrew/Cellar/gmp/6.3.0/include
# Library path for GMP
LFLAGS=-L/opt/homebrew/Cellar/gmp/6.3.0/lib
# Any libraries you want to link against
LIBS=-lgmp

# Define the C source files
SRCS=primegaps.c

# Define the C object files
OBJS=$(SRCS:.c=.o)

# Define the executable file
MAIN=primegaps

.PHONY: depend clean

all:    $(MAIN)
	@echo Compiled $(MAIN) successfully!

$(MAIN): $(OBJS) 
	$(CC) $(CFLAGS) $(INCLUDES) -o $(MAIN) $(OBJS) $(LFLAGS) $(LIBS)

# This is a suffix replacement rule for building .o's from .c's
# It uses automatic variables $<: the name of the prerequisite of
# the rule(a .c file) and $@: the name of the target of the rule (a .o file)
.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $<  -o $@

clean:
	$(RM) *.o *~ $(MAIN)

depend: $(SRCS)
	makedepend $(INCLUDES) $^

# ...

# Define the number of processes
NPROCS=4

run: $(MAIN)
	mpiexec -n $(NPROCS) ./$(MAIN)
