#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <stdlib.h>



int main(int argc,char *argv[]) {
   double x = 49.9 ; 
   double step = 0.1; 
   if ((x + step ) < 50.0 ) { 
       printf("x + step is less than 50.0\n" ); 
   }  
   return 1;
}
