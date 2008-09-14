#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <stdlib.h>

#define SIZE 4

int display_array_via_pointer(int *arr){

  printf("arr = %d, *arr = %d\n", arr, *arr);
  int i,j;
  int *ptr ;
  ptr = arr + 2  ;
  printf("ptr = %d, *ptr = %d\n", ptr, *ptr);

  ptr = (arr + 2) + SIZE  ;
  printf("ptr = %d, *ptr = %d\n", ptr, *ptr);
  for (i=0; i<SIZE;i++) { 
    for (j=0; j<SIZE;j++) { 
       ptr = arr + i*SIZE + j  ;
       printf("arr[%d][%d] = %d\n", i,j,*ptr);
       *ptr = j + i*j ;
       printf("arr[%d][%d] = %d\n", i,j,*ptr);
       
    }
  }
}


int main() {

  int a[SIZE][SIZE];
  int i,j;
  int *ptr;
  for (i=0; i<SIZE;i++) { 
    for (j=0; j<SIZE;j++) { 
      a[i][j] = (j+1) + (i+1)*(j+1) ;
      printf("a[%d][%d] = %d\n",i,j, a[i][j]);
    }
  }
  display_array_via_pointer(&a[0][0]);
  printf("Hello World");

}
