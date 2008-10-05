#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <stdlib.h>



int main(int argc,char *argv[]) {
   double x = 49.9 ; 
   double step = 0.1; 
   double threshold = 50.0;
   double next_value = x + step;
   if (next_value  < threshold  ) { 
      printf("x + step is less than 50.0, %f <  %f \n", (x + step ), threshold ); 
   }  
   double lessvalue = -585885210.00000; 
   double morevalue =  518647176.00000 ; 
    if (lessvalue  < morevalue  ) { 
      printf("less < more + step is less than 50.0, %f <  %f \n", (x + step ), threshold ); 
   }  
   
   return 1;
}
