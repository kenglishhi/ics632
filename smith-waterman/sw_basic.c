#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h> 

#define INDEX(X,Y,SIZE) ((X)*(SIZE))+(Y)


void print_matrix(int *matrix, int matrix_size) {
    int *current_value;
    int i,j;

    printf("matrix_size = %d \n\n", matrix_size  );
    for (i = 0; i < matrix_size; i++) {
        printf(" [" );
        for (j = 0; j < matrix_size; j++){
            current_value = matrix + i * matrix_size + j;
            printf(" %02d ", *current_value  );
        }
        printf("] " );

        printf("\n" );
    }
}

void init_edges(int *matrix, int matrix_size) {
    int i; 
    srand(time(0)); 
    int my_seed = getpid() ; 
    for (i=0; i< matrix_size; i++ ) { 
        matrix[INDEX(i,0, matrix_size)] = - (1+ (int) (50.0 * (rand() / (RAND_MAX + 1.0))));  
    } 
 
    for (i=0; i< matrix_size; i++ ) { 
        matrix[INDEX(0,i, matrix_size)] = - (1+ (int) (50.0 * (rand() / (RAND_MAX + 1.0))) ) ; 
    } 

}
int get_score(int top_left, int top, int left) { 
   int temp; 

//   if (top == left ) { 
//      return 0; 
//   } 
   if (top_left > top ) { 
     temp = top_left ; 
   } else {
     temp = top ; 
   } 

   if( temp > left) { 
      return temp; 
   } else { 
      return left; 
   } 

} 

int main(int argc, char **argv) {

    int matrix_size = 12 ; 

    struct timeval total_start; 
    struct timeval total_finish; 
  
    gettimeofday(&total_start,NULL); 

//    int row_size = matrix_size / nprocs;
//    double *matrix_a, *matrix_b, *result_matrix, *tempR, *tempS, *tempX;

    int *matrix_a; 

    matrix_a = (int *) calloc(matrix_size * (matrix_size), sizeof(int)) ;
    init_edges(matrix_a, matrix_size );  
    int i,j; 
    for (i=1; i < matrix_size; i++ ) { 
        for (j=1; j< matrix_size; j++ ) { 
            matrix_a[INDEX(i,j, matrix_size)] = get_score(matrix_a[INDEX(i-1,j-1, matrix_size)], matrix_a[INDEX(i-1,j, matrix_size)], matrix_a[INDEX(i,j-1, matrix_size)] ) ; 
//        printf("matrix_a[%d][%d] = %d \n", i, j, matrix_a[INDEX(i,j, matrix_size)] ); 
        } 
    } 
    print_matrix(matrix_a, matrix_size);     
    return 1; 
//    matrix_b = (int *) malloc(matrix_size * (row_size) * sizeof(int)) ;

}
