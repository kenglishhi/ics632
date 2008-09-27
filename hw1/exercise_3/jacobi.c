#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <stdlib.h>
#define N 20 

#### 
# jacobi iteration formula
#  n[i,j] = 0.25 * (m[i+1, j] +  m[i-1,j] + m[i,j+1] + m[i,j-1] 
### 

int main(int argc,char *argv[]) {
    double output[N][N]; 
    double m[N][N]; 
    int i, j;

    for (i=1; i<N; i++){
        for (j=0; j<N; j++){
            m[i][j] = 0.0;
        }
    }
    for (i=0; i<N; i++){
        m[i][0] = 1.0;
    } 

    for (i=1; i<N; i++){
        for (j=0; j<N; j++){
            output[i][j] = 0.0;
        }
    }
    for (i=0; i<N; i++){

   printf("Hello World\n");
   return 1;
}
