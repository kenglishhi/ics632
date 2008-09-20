#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <stdlib.h>


double mystery_function_800(double *x); 

int lgsearch(int trials, float step_size ) ; 
int lgsearch(int trials, float step_size ) { 
    double x[800]; 
    int i;

    for (i=0; i < 800 ; i++ ) { 
        x[i]  = (double) (50.0*rand()/(RAND_MAX+1.0));  
//        x[i]  = 49.8;
        printf( "Random number[%d] %1.5f \n", i, x[i] );
    } 
    printf("First Result => %10.5f", mystery_function_800(&x[0]) ) ;
    double prev_result = 0.0; 
    double cur_result = 0.0; 
    i=0; 
    int reset = 1; 
    do {
      if (reset) { 
        prev_result = mystery_function_800(&x[0]);   
        reset= 0; 
      } 
//      printf(" too big? %d\n", (x[i] + step_size -50.0) > 0.0 )  ;
      if ((x[i] + step_size) - 50.0 > 0.0)  { 
          printf("value too big"); 
          i++; 
          continue ;
      } 
      x[i] +=  step_size; 
      cur_result = mystery_function_800(&x[0]);   
//      printf( "arr[%d] %.5f (cur= %5.2f > prev=%5.5f) diff=%5.5f %d \n",i,  x[i], cur_result, prev_result, (cur_result - prev_result ),  (cur_result - prev_result ) < 0.0  );
//      printf( "-- cur_result %.5f > %.5f %d \n", cur_result, prev_result, (cur_result > prev_result)  );

      if ((cur_result - prev_result) > 0.0 ) { 
//        printf( "DOING STEP  %.5f > %.f \n", cur_result,prev_result  );
        prev_result = cur_result;   
      }  else { 
        x[i] -= step_size;
        i++; 
        reset= 1; 
      } 
      if (i%50 == 0 ) { 
         printf(".");
      } 
    } while (i < 800) ; 
    printf("\n");
    return 1;

} 


int main(int argc,char *argv[])
{

   double x = 50.04623; 
   double y = x + 0.1;
   if (y > 50.0) { 
      printf("too big"); 
   } 
   lgsearch(10, 0.1); 
   return 1;
}
