#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <stdlib.h>
#define N 20 


int main(int argc,char *argv[]) {
    double v[N][N]; 
    double b[N][N]; 
    int i, j;
    for (i=0; i<N; i++){
        for (j=0; j<N; j++){
            v[i][j] = 0.0;
            b[i][j] = 0.0;
        }
    }

   printf("Hello World\n");
   return 1;
}
