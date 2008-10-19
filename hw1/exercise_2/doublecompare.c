#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <stdlib.h>



int main(int argc,char *argv[]) {
   double x = 50.0 ; 
   double threshold = 50.0;
   if (x  == threshold ) { 
      printf("They are equal"); // x + step is less than 50.0, %f <  %f \n", (x + step ), threshold ); 
   }  
   return 1;
}
