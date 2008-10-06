#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <mpi.h>

#define MASTER_RANK 0
#define DEFAULT_TAG 0
#define DEBUG 0
#define NUMBER_OF_TRIALS 500

float get_time_diff(struct timeval *s, struct timeval *e) ;
void MPI_MyBcast(int *buffer, int count, int root, MPI_Comm comm)  ; 
void randomize_array(int *array, int array_size) ; 
void print_array(int *array, int array_size, int rank  ) ; 

int main(int argc, char **argv) {
    int rank, nprocs, i;
    struct timeval start;
    struct timeval end;

    // Initialize the MPI, get the size and the rank.
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

    int array_size = atoi(argv[1]);
    srand(time(0));

    printf("Array Size,%d\n", array_size );
    int *data_array ;
    data_array = (int *) malloc(array_size * sizeof(int));
    if (rank == MASTER_RANK) {
        randomize_array(data_array, array_size) ; 
        if (DEBUG) {
            print_array(data_array, array_size, rank) ; 
        }
        printf(" ----------------- \n");

    } 
   // RUN THE MPI_MyBcast method
    gettimeofday(&start,NULL);
    for (i=0; i< NUMBER_OF_TRIALS; i++) { 
        MPI_Bcast(data_array, array_size, MPI_INT, MASTER_RANK, MPI_COMM_WORLD);
    }
    gettimeofday(&end,NULL);
//    if (rank == MASTER_RANK) { 
        printf("Process, %d, MPI_Bcast,%f\n", rank, get_time_diff(&start, &end));
//    }

    // RUN THE MPI_MyBcast method
    gettimeofday(&start,NULL);
    for (i=0; i< NUMBER_OF_TRIALS; i++) { 
        MPI_MyBcast(data_array, array_size, MASTER_RANK, MPI_COMM_WORLD); 
    } 
    gettimeofday(&end,NULL);
//    if (rank == MASTER_RANK) { 
        printf("Process,%d, MPI_MyBcast,%f\n", rank, get_time_diff(&start, &end));
//    }

    if (DEBUG) {
        print_array(data_array, array_size, rank) ; 
    }
 
    MPI_Finalize();
    return 0;

}
