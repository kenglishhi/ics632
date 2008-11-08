/* sw_paralle.c */

#include  <stdio.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <mpi.h>

#define STRLEN     8
#define ITER       1
#define GAP   -1
#define MATCH 1
#define MISMATCH -1
#define SEQ1_LEN 8
#define DIRECTION_NONE     0
#define DIRECTION_UP       1
#define DIRECTION_LEFT     2
#define DIRECTION_DIAGONAL 3
#define GLOBAL_ROW_NUMBER(I,RANK,ROW_SIZE) ( (RANK)*(ROW_SIZE)+(I) )
#define ARRAY_OFFSET(I,J,NROWS)  ((I)*(NROWS) + (J))  


void print_score_matrix(int *matrix, int row_size, int col_size) {
   int rank, nprocs;  
   MPI_Comm_rank(MPI_COMM_WORLD, &rank);
   MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
   int *current_value;
   int i,j;

   printf("row_size , col_size = [%d,%d]  \n\n", row_size, col_size );
   for (i = 0; i < row_size; i++) {
        printf(" [" );
        for (j = 0; j < col_size; j++){
            current_value = matrix + (i * col_size) + j;
            printf("(%d,%d)=%d ",  GLOBAL_ROW_NUMBER(i,rank,row_size), j, *current_value  );
        }
        printf("] " );

        printf("\n" );
    }
}

void calculate_chunk(int *seq1_arr, int *seq2_arr, int *score_matrix, int nrows,int ncols, int col_start, int chunk_size) {
   int rank, nprocs;  
   MPI_Comm_rank(MPI_COMM_WORLD, &rank);
   MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
//   printf("Calculating chunk= %d, col_start\n", col_start);  

   int row_start=0; 
   if (rank ==0 ) { 
      row_start=1; 
   } 
   if (col_start ==0 ) {
     col_start=1; 
   } 
   int i,j;
   int  diagonal_score, left_score, up_score;
   int letter1, letter2 ; 
   int max_i, max_j, max_score; 
   for (i = row_start; i < nrows;  i++ ) { 
     for (j = col_start; j < col_start+chunk_size; j++ ) { 


       diagonal_score=0; left_score=0; up_score=0;      
       letter1 = *(seq1_arr+ (i-1) ) ; 
       letter2 = *(seq2_arr +(j-1) ); 
       // calculate match score 
       
       if (letter1 == letter2)  { 
         diagonal_score = *(score_matrix + ARRAY_OFFSET(i-1, j-1, ncols)) + MATCH; 
       } 
       else { 
         diagonal_score = *(score_matrix + ARRAY_OFFSET(i-1, j-1, ncols))  + MISMATCH; 
       } 
       
       // calculate gap scores
       up_score   = *(score_matrix + ARRAY_OFFSET(i-1, j, ncols)  )  + GAP;
       left_score = *(score_matrix + ARRAY_OFFSET(i, j-1, ncols)  )  + GAP;
       if (i==1 && j==1) {        
       printf("%d [%d,%d] diagonal_score: %d, up_score: %d, left_score: %d,letter1=%d, letter2=%d \n", ARRAY_OFFSET(i,j,ncols), i, j, diagonal_score, up_score, left_score, letter1, letter2 ); 
       }
       if ((diagonal_score <= 0) && (up_score <= 0) && (left_score <= 0)) {
            *(score_matrix + ARRAY_OFFSET(i, j, ncols) )    = 0;
//            printf("[%d,%d] diagonal_score: %d, up_score: %d, left_score: %d, score_matrix:%d \n", i, j, diagonal_score, up_score, left_score, score_matrix[i][j]); 
//            direction_matrix[i][j]   = DIRECTION_NONE;
            continue; 
        }

        // choose best score
        if (diagonal_score >= up_score) {
            if (diagonal_score >= left_score) {
              *(score_matrix + ARRAY_OFFSET(i, j, ncols) )    = diagonal_score;
//              direction_matrix[i][j]   = DIRECTION_DIAGONAL;
                //$matrix[$i][$j]{pointer} = "diagonal";
            }
            else {
              *(score_matrix + ARRAY_OFFSET(i, j, ncols) )    = left_score;
//              direction_matrix[i][j]   = DIRECTION_LEFT;
                //$matrix[$i][$j]{pointer} = "left";
            }
        } else {
            if (up_score >= left_score) {
              *(score_matrix + ARRAY_OFFSET(i, j, ncols) )   = up_score;
//              direction_matrix[i][j]   = DIRECTION_UP;
                //$matrix[$i][$j]{pointer} = "up";
            }
            else {
              *(score_matrix + ARRAY_OFFSET(i, j, ncols) )    = left_score;
//              direction_matrix[i][j]   = DIRECTION_LEFT;
                //$matrix[$i][$j]{pointer} = "left";
            }
        }

//       printf("[%d,%d] diagonal_score: %d, up_score: %d, left_score: %d, score_matrix:%d \n", i, j, diagonal_score, up_score, left_score, score_matrix[i][j]); 

        // set maximum score
        if (*(score_matrix + ARRAY_OFFSET(i, j, ncols) )  > max_score) {
            max_i     = i;
            max_j     = j;
            max_score = *(score_matrix + ARRAY_OFFSET(i, j, ncols) ) ;
        }







     }
     printf("\n" );  
   } 

}

int  main(int argc,char *argv[]) { 
  MPI_Init(&argc, &argv);  
  int rank, nprocs;  
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

  char  alphabet[21] = "acdefghiklmnpqrstvwy"; 
  char *seq1 ; 
  char *seq2 ; 

  if (argc < 3) { 
     printf("You need 2 arguments\n"); 
     return 0 ;
  } else { 
     seq1 = argv[1] ; 
     seq2 = argv[2] ; 
  } 
   

  printf("seq1 = %s\n", seq1); 
  printf("seq2 = %s\n", seq2); 
  
  int seq1_length = 0 ; 
  int seq2_length = 0; 
  while (seq1[seq1_length] != '\0') { 
    seq1_length++;  
  } 
//  printf("seq1_length = %d\n", seq1_length ); 
  while (seq2[seq2_length] != '\0') { 
    seq2_length++;  
  } 
//  printf("seq2_length = %d\n", seq2_length ); 

//  char *seq1 = "ppeaiccc"; 
//  char *seq2 = "ggeaicgg"; 

  int seq1_arr[seq1_length], seq2_arr[seq2_length]  ;
  int i, j; 

/* OLD
  int score_matrix[seq2_length+1][seq1_length+1] ;  
  int direction_matrix[seq2_length+1][seq1_length+1] ;  
*/

  int score_matrix[(seq2_length+1)/2][(seq1_length+1)] ;  
  int direction_matrix[(seq2_length+1)/2][(seq1_length+1)] ;  

  int max_i=-1, max_j=-1, max_score=-1 ; 
  int diagonal_score, left_score, up_score  ;
  int letter1, letter2; 

  for (i=0; i < seq1_length; i++) { 
    for (j=0; j<20; j++) { 
      if (seq1[i] == alphabet[j]){ 
        seq1_arr[i] = j;
      } 
   } 
  }
  int l; 
  for (l=0; l < seq2_length; l++) {
    for (j=0; j<20; j++) { 
      if (seq2[l] == alphabet[j]) { 
        seq2_arr[l] = j;
      } 
    }
  }

//  printf("seq2_arr[0] = %d\n",  seq2_arr[0]);//   = seq1_length  = %d\n", seq1_length); 
  if (rank ==0) { 
    for (j=0; j <= seq1_length; j++) { 
      score_matrix[0][j] = 0; 
      direction_matrix[0][j]   = DIRECTION_NONE;
    } 
  } 

  for (i=0; i < (seq2_length+1)/2; i++ ) { 
    score_matrix[i][0] = 0; 
    direction_matrix[i][0]   = DIRECTION_NONE;
  } 
  int nrows =  (seq1_length+1)/2; 
  int ncols =  (seq1_length+1); 
  int chunk_size = (seq1_length+1)/4;
  int number_of_chucks= (seq1_length+1)/chunk_size;
  int chunk; 
  if (rank ==0 ) { 
     calculate_chunk(&seq1_arr[0], &seq2_arr[0], &score_matrix[0][0], nrows, ncols, 0, chunk_size);
     print_score_matrix(&score_matrix[0][0],  (seq1_length+1)/2, (seq1_length+1)  ); 
//     send_chunk
       
  } else {
//     recv_chunk( 
  } 

//  for (chunk =0; chunk < number_of_chucks ; chunk++) { 
//       calculate_chunk(&score_matrix[0][0], nrows, chunk*chunk_size, chunk_size);
//  } 
  
  

//  printf("cols = seq2_length  = %d\n", seq2_length);  
   
//   print_score_matrix(&score_matrix[0][0],  (seq2_length+1)/2, (seq1_length+1)/2  ); 
   return 0; 
/*******************************
 * END TEST HERE
 *
********************************/

  for (i=1; i <= seq2_length; i++) { 
//    printf("score_matrix[%d][0]  %d\n", i, score_matrix[i][0] ); 
    for (j=1; j <= seq1_length; j++) { 

       diagonal_score=0; left_score=0; up_score=0;      
       letter1 = seq1_arr[i-1]; 
       letter2 = seq2_arr[j-1]; 
       // calculate match score 
       
       if (letter1 == letter2)  { 
         diagonal_score = score_matrix[i-1][j-1] + MATCH; 
       } 
       else { 
         diagonal_score = score_matrix[i-1][j-1] + MISMATCH; 
       } 
       
       // calculate gap scores
       up_score   = score_matrix[i-1][j] + GAP;
       left_score = score_matrix[i][j-1] + GAP;
          
//       printf("[%d,%d] diagonal_score: %d, up_score: %d, left_score: %d, \n", i, j, diagonal_score, up_score, left_score ); 
       if ((diagonal_score <= 0) && (up_score <= 0) && (left_score <= 0)) {
            score_matrix[i][j]   = 0;
//            printf("[%d,%d] diagonal_score: %d, up_score: %d, left_score: %d, score_matrix:%d \n", i, j, diagonal_score, up_score, left_score, score_matrix[i][j]); 
            direction_matrix[i][j]   = DIRECTION_NONE;
            continue; 
        }

        // choose best score
        if (diagonal_score >= up_score) {
            if (diagonal_score >= left_score) {
              score_matrix[i][j]   = diagonal_score;
              direction_matrix[i][j]   = DIRECTION_DIAGONAL;
                //$matrix[$i][$j]{pointer} = "diagonal";
            }
            else {
              score_matrix[i][j]   = left_score;
              direction_matrix[i][j]   = DIRECTION_LEFT;
                //$matrix[$i][$j]{pointer} = "left";
            }
        } else {
            if (up_score >= left_score) {
              score_matrix[i][j]   = up_score;
              direction_matrix[i][j]   = DIRECTION_UP;
                //$matrix[$i][$j]{pointer} = "up";
            }
            else {
              score_matrix[i][j]   = left_score;
              direction_matrix[i][j]   = DIRECTION_LEFT;
                //$matrix[$i][$j]{pointer} = "left";
            }
        }

//       printf("[%d,%d] diagonal_score: %d, up_score: %d, left_score: %d, score_matrix:%d \n", i, j, diagonal_score, up_score, left_score, score_matrix[i][j]); 

        // set maximum score
        if (score_matrix[i][j] > max_score) {
            max_i     = i;
            max_j     = j;
            max_score = score_matrix[i][j];
        }


    }  
//    printf("\n");
  } 


//printf("max_i = %d\n", max_i); 
//printf("max_j = %d\n", max_j); 
//printf("max_score = %d\n", max_score); 


print_score_matrix(&score_matrix[0][0],  seq2_length+1, seq1_length+1  ); 
// printf("finished printing score matrix= %d\n", max_score); 
//  trace-back

//char align1[SEQ1_LEN] = "ppeaiccc";
//char align2[SEQ1_LEN] = "ppeaiccc";
int align1_arr[seq1_length], align2_arr[seq2_length]  ;

//align1 = "";
//align2 = "";

j = max_j; i = max_i;

int align1_index=0; 
int align2_index=0; 

while (1) {
    if (direction_matrix[i][j] == DIRECTION_NONE) 
      break ; 

    if (direction_matrix[i][j] ==  DIRECTION_DIAGONAL ) {
         align1_arr[align1_index] = seq1_arr[j-1]  ;
         align2_arr[align2_index] = seq2_arr[i-1]  ;
//        $align1 .= substr($seq1, $j-1, 1);
//        $align2 .= substr($seq2, $i-1, 1);
        i--; j--;
    }
    else if (direction_matrix[i][j] ==  DIRECTION_LEFT  ) {
         align1_arr[align1_index] = seq1_arr[j-1]  ;
         align2_arr[align2_index] = -1 ; 
//        $align1 .= substr($seq1, $j-1, 1);
//        $align2 .= "-";
        j--;
    }
    else if (direction_matrix[i][j] ==  DIRECTION_UP ) {
         align1_arr[align1_index] = -1; 
         align2_arr[align2_index] = seq2_arr[i-1]  ;
//        align1 .= "-";
//        align2 .= substr($seq2, $i-1, 1);
        i--;
    }
    align1_index++; align2_index++; 

}
// printf("align1_index = %d, align2_index = %d\n", align1_index, align2_index); 
char *align1 ; 
char *align2; 
align1 = (char *) calloc(seq1_length,sizeof(char) )  ; 
align2 = (char *) calloc(seq2_length,sizeof(char) )  ; 
for(i=0;i<align1_index;i++){ 
  align1[align1_index-i-1] =  alphabet[align1_arr[i]] ; 
} 

printf("{%d} align1 = %s\n",rank, align1); 
printf("{%d} align2 = %s\n",rank, align1); 


/*
$align1 = reverse $align1;
$align2 = reverse $align2;
print "$align1\n";
print "$align2\n"

*/
return 1; 
}

