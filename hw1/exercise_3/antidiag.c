#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <stdlib.h>
#define INDEX(X,Y,SIZE) (X*SIZE)+Y


// this is test code to show how to calculate the anti-diagnol

int main(int argc,char *argv[] )
{
    int size = 4;
    if (argc > 1) {
        size = atoi(argv[1]);
    }

    int i;
    int j;
    int diagonol_length = size+size-1; 
    int start, end; 
    
    for (i=0; i < diagonol_length ; i++ ) {
	printf("diag: ");
	if (i < size) {
            start =0;     
            end = i+1 ; 
        } else { 
            start = i-size+1 ; 
            end = diagonol_length-size+1  ; 
        } 
        for (j=start; j < end; j++) {
            printf("[%d,%d]", j, i-j);
        }
	printf("\n");
    }
}


