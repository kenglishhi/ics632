#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <stdlib.h>

#define ARRAY_SIZE 800


static float getTimeDiff(struct timeval *s, struct timeval *e);

static float getTimeDiff(struct timeval *s, struct timeval *e) {
    struct timeval diff_tv;

    diff_tv.tv_usec = e->tv_usec - s->tv_usec;
    diff_tv.tv_sec = e->tv_sec - s->tv_sec;

    if (s->tv_usec > e->tv_usec) {
        diff_tv.tv_usec += 1000000;
        diff_tv.tv_sec--;
    }

    return (float) diff_tv.tv_sec + ((float) diff_tv.tv_usec / 1000000.0);
}

double mystery_function_800(double *x); 

int lgsearch(float step_size ) ; 
int lgsearch(float step_size ) { 
    double x[ARRAY_SIZE]; 
    int i;
    struct timeval start;
    struct timeval end;


    for (i=0; i < ARRAY_SIZE ; i++ ) { 
        x[i]  = (double) (50.0*rand()/(RAND_MAX+1.0));  
//        x[i]  = 49.8;
//        printf( "Random number[%d] %1.5f \n", i, x[i] );
    } 
    printf("First Result => %10.5f\n", mystery_function_800(&x[0]) ) ;
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
//      printf(" too big? %d\n", (x[i] + step_size -50.0) > 0.0 )  ;
      if ((x[i] + step_size) - 50.0 > 0.0)  { 
          i++; 
          continue ;
      } 
      x[i] +=  step_size; 
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
    printf("Time difference for implementation %.5f seconds\n", getTimeDiff(&start, &end));

    return 1;

} 


int main(int argc,char *argv[])
{
   srand(200000);
   if (argc < 3) { 
      printf("Usage: lgsearch.exe <number of trials> <strep size>\n ");
     
      return -1;
   } 
   int trials = atoi(argv[1]);
   double step_size = atoi(argv[2]);
   printf("CALLED lgsearch.exe %d %.5f\n\n" , trials, step_size);
   int i ;
   for (i = 0; i < trials; i++) 
      lgsearch(0.1); 

   return 1;
}
