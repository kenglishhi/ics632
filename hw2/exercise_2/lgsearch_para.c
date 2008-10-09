#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include <unistd.h>
#include <mpi.h>

#define MASTER_RANK 0
#define DEFAULT_TAG 0
#define GO_SIGNAL 400
#define TERM_SIGNAL 404
#define MINIMUM_INDEX 0
#define TIME_INDEX 1


float get_time_diff(struct timeval *s, struct timeval *e) ;

double mystery_function_800(double *x) ;
double lgsearch(double step_size, int trials ) ;

void signal_worker(MPI_Request *request,double *result_data, int proc) {
    int x = GO_SIGNAL ;
    MPI_Isend(&x, 1, MPI_INT, proc, DEFAULT_TAG, MPI_COMM_WORLD, request);
    MPI_Irecv(result_data, 2, MPI_DOUBLE, proc, DEFAULT_TAG, MPI_COMM_WORLD, request );
}
int is_worker_done(MPI_Request *request) {
    MPI_Status status  ;
    int flag ;
    int result = MPI_Test( request,&flag, &status);
    if (flag) {
	MPI_Wait( request, &status);
	return 1;
    } else {
	return 0;
    }
}
double do_dispatch(int trials) {
    int nprocs,  proc=1;
    double minimum = -1.0  ;
    int rank;
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Request requests[nprocs]  ;
    double work_times[nprocs]  ;
    MPI_Status status  ;
    int x = GO_SIGNAL ;
    double data[nprocs][2] ;
    int completed_flags[nprocs] ;
    int idle_workers[nprocs] ;

    for (proc=0; proc< nprocs ; proc++) {
	requests[proc] = NULL;
	work_times[proc] = 0.0;
        is_idle_workder[proc] = 1; 
	data[nprocs][MINIMUM_INDEX] =0.0 ;
	data[nprocs][TIME_INDEX] =0.0 ;
    }
    is_idle_workder[MASTER_RANK] = 0; 

    printf("%d\tDoing dispatch : %d MPI_ERR_REQUEST  :  %d \n", rank, x, MPI_ERR_REQUEST  );

    int trials_completed = 0;
    int workers_are_still_working = 1;
    int do_trial = 0 ;
    while (workers_are_still_working) {
	// find the next idle worker

	if (trials_completed < trials )  {
	    for (proc=0; proc < nprocs ; proc++) {
		if (proc != MASTER_RANK) {
		    if (requests[proc] == NULL ) {
			signal_worker(&requests[proc],&data[proc][MINIMUM_INDEX], proc) ;
                        break;
		    } else {
			if (is_worker_done(&requests[proc]))  {
			    trials_completed++;
			    work_times[proc] +=  data[proc][TIME_INDEX];

			    printf("%d\tTrial: %d \n", rank, trials_completed);
			    printf("%d\tprocces : %d , minimum: %f, time: %f cum_time: %f \n", rank, proc, data[proc][MINIMUM_INDEX],  data[proc][TIME_INDEX], work_times[proc]   );
			    if (data[proc][MINIMUM_INDEX]  <  minimum ) {
				minimum = data[proc][MINIMUM_INDEX] ;
				printf("%d\tNew Minimum : %f  \n", rank, minimum );
			    }
			    if (trials_completed < (trials) ) {
				signal_worker(&requests[proc],&data[proc][MINIMUM_INDEX], proc) ;
			    } else { 
                               idle_
                            } 
			    break;
			}
		    }
	        }
	    }
         } else {
	   workers_are_still_working = 0;
	   for (proc=0; proc < nprocs ; proc++) {
		if (proc != MASTER_RANK) {
            	   if (!is_worker_done(&requests[proc]))  {
             	       workers_are_still_working = 1;
          	   } else {
			    work_times[proc] +=  data[proc][TIME_INDEX];
                   } 
               } 
           }
	}
    }
    int sum_completed ;
/*    while (workers_are_still_working) {
	sum_completed  =0;
	for (proc=0; proc< nprocs ; proc++) {
	    if (proc != MASTER_RANK) {
		MPI_Test( &requests[proc],&completed_flags[proc], &status);
		sum_completed += completed_flags[proc];
		if (completed_flags[proc] ) {
		    trials_completed++;
		    MPI_Wait( &requests[proc], &status);
		    if (data[proc][MINIMUM_INDEX]  <  minimum ) {
			minimum = data[proc][MINIMUM_INDEX] ;
			printf("%d\tNew Minimum : %f  \n", rank, minimum );
		    }
		    printf("%d\tProcess %d Time : %f  \n", rank, proc, data[proc][TIME_INDEX]);
		    work_times[proc] +=  data[proc][TIME_INDEX];
		}

	    }
	}
	if (sum_completed == nprocs -1) {
	    printf("Workers are done  : %d \n", sum_completed );
	    workers_are_still_working =0;
	}
    }
*/


//    trials_completed++;
////    printf("Trials COMPLETED  : %d \n", trials_completed );
////    }
    for (proc=0; proc< nprocs ; proc++) {
	if (proc != MASTER_RANK) {
	    printf("%d\tProccess %d time %f \n", rank, proc, work_times[proc] );
	}
    }

    printf("%d\tDone looping, sending term signal to waiting procs\n", rank);
    x = TERM_SIGNAL;
    for (proc=0; proc< nprocs ; proc++) {
	if (proc != MASTER_RANK) {
	    MPI_Ssend(&x, 1, MPI_INT, proc, DEFAULT_TAG, MPI_COMM_WORLD);
	}
    }
    printf("%d\tReturning from dispatch\n", rank);
//   printf("Received %f from %d", data, proc);
    return minimum ;

}

void do_work(double step_size) {
    int rank, x = 666;
    double data[2] ;
    data[MINIMUM_INDEX] = 0.0; data[TIME_INDEX] = 0.0;
    struct timeval start;
    struct timeval finish;

    MPI_Status status  ;
    MPI_Request request  ;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    while (1) {
	MPI_Recv(&x, 1, MPI_INT, MASTER_RANK, DEFAULT_TAG, MPI_COMM_WORLD, &status);
	if (x == GO_SIGNAL){
	    gettimeofday(&start,NULL);

	    printf("%d\tDoing work new_ buffer %d \n", rank, x);
	    data[MINIMUM_INDEX] = lgsearch(step_size, 1);
	    gettimeofday(&finish,NULL);
	    data[TIME_INDEX] = get_time_diff(&start, &finish);

	    printf("%d\tDone Working%d \n", rank, x);
	    MPI_Isend(&data[0], 2, MPI_DOUBLE, MASTER_RANK, DEFAULT_TAG, MPI_COMM_WORLD, &request);
	} else {
	    printf("%d\tGot Term signal \n", rank);
	    break;
	}
    }

}


int main(int argc,char *argv[]) {
    int rank, nprocs, i;

    // Initialize the MPI, get the size and the rank.
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

    if (argc < 3) {
	printf("Usage: lgsearch.exe <number of trials> <strep size>\n");
	return -1;
    }

    struct timeval start;
    struct timeval finish;

    int trials = atoi(argv[1]);
    double step_size = strtod(argv[2], NULL);
    //float time_spent;
//    printf("trials,%d, step_size,%f\n",trials, step_size);

    gettimeofday(&start,NULL);
    if (rank == MASTER_RANK) {
	do_dispatch(trials) ;
    } else {
	do_work(step_size);
    }

    MPI_Finalize();

    return 0;

}
