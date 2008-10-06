#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <mpi.h>

#define MASTER_RANK 0
#define DEFAULT_TAG 0
#define DEBUG 0

void MPI_MyBcast(int *buffer, int count, int root, MPI_Comm comm)  ; 
void randomize_array(int *array, int array_size) ; 
void print_array(int *array, int array_size, int rank  ) ; 

 

int main(int argc, char **argv) {
    int rank, nprocs;
    // Initialize the MPI, get the size and the rank.
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

    int array_size = atoi(argv[1]);
    srand(time(0));

    int *data_array ;
    data_array = (int *) malloc(array_size * sizeof(int));
    if (rank == MASTER_RANK) {
        randomize_array(data_array, array_size) ; 
//	for (i=0; i< array_size; i++) {
//            data_array[i] = rand(); 
//	}

        if (DEBUG) {
            print_array(data_array, array_size, rank) ; 
        }
        printf(" ----------------- \n");

    } 
    MPI_MyBcast(data_array, array_size, MASTER_RANK, MPI_COMM_WORLD); 
    if (DEBUG) {
        print_array(data_array, array_size, rank) ; 
    }
    randomize_array(data_array, array_size) ; 
    printf(" ----------------- \n");

    MPI_Bcast(data_array, array_size, MPI_INT, MASTER_RANK, MPI_COMM_WORLD);
    if (DEBUG) {
        print_array(data_array, array_size, rank) ; 
    }
    
    MPI_Finalize();
    return 0;

}
