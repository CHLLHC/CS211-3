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
	int id = 0, p = 1;
	MPI_Comm_rank(MPI_COMM_WORLD, &id);
	MPI_Comm_size(MPI_COMM_WORLD, &p);

	if (argc != 2) {
		if (id == 0)
			printf("Command line: %s <m>\n", argv[0]);
		MPI_Finalize();
		return 1;
	}
	//NO OFFSET
	uint64_t n = atoll(argv[1]);
	uint64_t proc0_size = n / p;
	if (proc0_size < sqrt(n)) {
		if (id == 0)
			printf("Too many processes\n");
		MPI_Finalize();
		return 1;
	}

	uint64_t test = n;
	for (uint64_t i = 0;i<1E10;++i){
		MPI_Bcast(&test, 1, MPI_UINT64_T, 0, MPI_COMM_WORLD);
	}

	elapsed_time += MPI_Wtime();


	if (id == 0) {
		printf("The total number of prime: %d, total time: %10.6f, total node ", global_count, elapsed_time);
		//printf("%d primes are less than or equal to %llu\n",
		//	global_count, n);
		//printf("Total elapsed time: %10.6f\n", elapsed_time);
		//printf("This is Part 1\n");
	}

	MPI_Finalize();
	return 0;
}
