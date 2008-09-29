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

    int x;
    int y;
    int diagonol_length = size+size-1; 
    int start, end; 
    
    for (x=0; x < diagonol_length ; x++ ) {
	printf("diag: ");
	if (x < size) {
            start =0;     
            end = x+1 ; 
        } else { 
            start = x-size+1 ; 
            end = diagonol_length-size+1  ; 
        } 
        for (y=start; y < end; y++) {
            printf("[%d,%d]", y+1, x-y+1);
        }
	printf("\n");
    }
}


