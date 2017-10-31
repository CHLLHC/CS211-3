#include "mpi.h"
#include <cmath>
#include <cstdio>
#include <cstring>
#include <cstdint>
#include <cstdlib>
#define MIN(a,b)  ((a)<(b)?(a):(b))
#define BLOCK_LOW(id,p,n)  ((id)*(n)/(p))

#define BLOCK_HIGH(id,p,n) \
        ( BLOCK_LOW((id)+1,p,n)-1 ) 

#define BLOCK_SIZE(id,p,n) \
        (BLOCK_LOW( (id)+1, p, n) -  BLOCK_LOW((id), p, n))

#define BLOCK_OWNER(index,p,n) \
        ( ( ((p)*(index)+1)-1 ) / (n) )


int main(int argc, char *argv[])
{
	int rc = MPI_Init(&argc, &argv);
	MPI_Barrier(MPI_COMM_WORLD);
	double elapsed_time = -MPI_Wtime();
	int id, p;
	MPI_Comm_rank(MPI_COMM_WORLD, &id);
	MPI_Comm_size(MPI_COMM_WORLD, &p);

	if (argc != 2) {
		if (id == 0)
			printf("Command line: %s <m>\n", argv[0]);
		MPI_Finalize();
		return 1;
	}

	int n = atoi(argv[1]);
	int low_value = 2 + BLOCK_LOW(id, p, n - 1);
	int high_value = 2 + BLOCK_HIGH(id, p, n - 1);
	int size = BLOCK_SIZE(id, p, n - 1);
	int proc0_size = (n - 1) / p;
	if ((2 + proc0_size) < (int)sqrt((double)n)) {
		if (id == 0) printf("Too many processes\n");
		MPI_Finalize();
		return 1;
	}

	uint8_t *marked = new uint8_t[size];
	if (marked == NULL) {
		printf("Cannot allocate enough memory\n");
		MPI_Finalize();
		return 1;
	}

	for (int i = 0; i < size; i++)
		marked[i] = 0;
	int index;
	if (!id)
		index = 0;
	int prime = 2;
	int first;
	while (prime * prime <= n) {
		if (prime * prime > low_value)
			first = prime * prime - low_value;
		else {
			if (!(low_value % prime)) first = 0;
			else first = prime - (low_value % prime);
		}
		for (int i = first; i < size; i += prime)
			marked[i] = 1;
		if (id == 0) {
			while (marked[++index]);
			prime = index + 2;
		}
		MPI_Bcast(&prime, 1, MPI_INT, 0, MPI_COMM_WORLD);
	}
	int count = 0;
	for (int i = 0; i < size; i++)
		if (!marked[i]) count++;
	int global_count = 0;
	MPI_Reduce(&count, &global_count, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
	elapsed_time += MPI_Wtime();
	if (id == 0) {
		printf("%d primes are less than or equal to %d\n",
			global_count, n);
		printf("Total elapsed time: %10.6f\n", elapsed_time);
	}
	MPI_Finalize();
	return 0;
}
