#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <stdlib.h>

#define ARRAY_SIZE 800
float get_time_diff(struct timeval *s, struct timeval *e) ;

int lgsearch(double step_size ) {
    double x[ARRAY_SIZE];
    int i;
    struct timeval start;
    struct timeval end;


    for (i=0; i < ARRAY_SIZE ; i++ ) {
	x[i]  = (double) (50.0*rand()/(RAND_MAX+1.0));
//        x[i]  = 49.8;
//        printf( "Random number[%d] %1.5f \n", i, x[i] );
    }
//    printf("First Result => %10.5f\n", mystery_function_800(&x[0]) ) ;
    gettimeofday(&start,NULL);

    double prev_result = 0.0;
    double cur_result = 0.0;
    i = 0;
    int reset = 1;
    do {
	if (reset) {
	    prev_result = mystery_function_800(&x[0]);
	    reset= 0;
	}

//      printf(" too big? %d\n", ((x[i] + step_size -50.0) > 0.0) )  ;

	int var = (int) (x[i] + step_size)  ;
//      printf( " var  = %d (%2.5f + %2.5f) \n", var, x[i], step_size ) ;
	if (var >= 50)  {
	    i++;
	    continue ;
	}
	x[i] +=  step_size;
//      printf( " value = %.5f -- cur_result %.5f > %.5f %d \n", x[i], cur_result, prev_result, (cur_result > prev_result)  );
	cur_result = mystery_function_800(&x[0]);
//      printf( "arr[%d] %.7f prev =  %5.7f , cur = %5.5f \n",i,  x[i], prev_result, cur_result);
//      printf( "-- cur_result %.5f > %.5f %d \n", cur_result, prev_result, (cur_result > prev_result)  );

	if ((cur_result - prev_result) > 0.0 ) {
//       printf( "DOING STEP  %.7f > %.7f \n", cur_result,prev_result  );
	    prev_result = cur_result;
	}  else {
	    x[i] -= step_size;
	    i++;
	    reset= 1;
	    printf(".");
	    fflush(stdout);
	}
    } while (i < ARRAY_SIZE) ;
    printf("\n");
    gettimeofday(&end,NULL);
    printf("Time Spent %f seconds\n", get_time_diff(&start, &end));

    return 1;
}


