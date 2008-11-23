/* sw_sequential.c */

#include <stdio.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>


#define STRLEN     8
#define   ITER       1
#define GAP   -1
#define MATCH 1
#define MISMATCH -1
#define SEQ1_LEN 8
#define DIRECTION_NONE     0
#define DIRECTION_UP       1
#define DIRECTION_LEFT     2
#define DIRECTION_DIAGONAL 3


void print_score_matrix(int *matrix, int nrows, int ncols) {
   int *current_value;
   int i,j;

   printf("nrows , col_size = [%d,%d]  \n\n", nrows, ncols );
   for (i = 0; i < nrows; i++) {
        printf(" [" );
        for (j = 0; j < ncols; j++){
            current_value = matrix + (i * ncols) + j;
            printf("(%d,%d)=%d ", i, j, *current_value  );
        }
        printf("] " );

        printf("\n" );
    }
}


int  main(int argc,char *argv[]) { 
  char  alphabet[21] = "acdefghiklmnpqrstvwy"; 
  char *seq1;  
  char *seq2;  
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
  printf("seq1_length = %d\n", seq1_length ); 
  while (seq2[seq2_length] != '\0') { 
    seq2_length++;  
  } 
  printf("seq2_length = %d\n", seq2_length ); 

//  char *seq1 = "ppeaiccc"; 
//  char *seq2 = "ggeaicgg"; 

  int seq1_arr[seq1_length], seq2_arr[seq2_length]  ;
  int i, j; 
  int score_matrix[seq2_length+1][seq1_length+1] ;  
  int direction_matrix[seq2_length+1][seq1_length+1] ;  
  int max_i=-1, max_j=-1, max_score=-1 ; 
  int diagonal_score, left_score, up_score  ;
  int letter1, letter2; 

  /* Convert residue characters to indices */
  printf("STRLEN = %d\n", STRLEN);
  printf("d_loc = %d\n", STRLEN);
  printf("seq1 = %s\n", seq1); 
  printf("seq2 = %s\n", seq2); 
  for (i=0; i < seq1_length; i++)
    for (j=0; j<20; j++)
      if (seq1[i] == alphabet[j])
        seq1_arr[i] = j;

  for (i=0; i < seq2_length; i++) 
    for (j=0; j<20; j++) 
      if (seq2[i] == alphabet[j]) 
        seq2_arr[i] = j;

  for (j=0; j <= seq1_length; j++) { 
    score_matrix[0][j] = 0; 
    direction_matrix[0][j]   = DIRECTION_NONE;
  } 
  printf("rows = seq1_length  = %d\n", seq1_length); 
  for (i=0; i <= seq2_length; i++ ) { 
    score_matrix[i][0] = 0; 
    direction_matrix[i][0]   = DIRECTION_NONE;
  } 
  printf("cols = seq2_length  = %d\n", seq2_length); 

  for (i=1; i <= seq2_length; i++) { 
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
          
//       if (i==1 && j==1) { 
//          printf("[%d,%d] diagonal_score: %d, up_score: %d, left_score: %d, letter1=%d, letter2=%d \n", i, j, diagonal_score, up_score, left_score, letter1, letter2 ); 
//            printf("%d [%d,%d] diagonal_score: %d, up_score: %d, left_score: %d,letter1=%d, letter2=%d \n", ARRAY_OFFSET(i,j,ncols), i, j, diagonal_score, 
//                up_score, left_score, letter1, letter2 );
 //      }
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
  } 


printf("max_i = %d\n", max_i); 
printf("max_j = %d\n", max_j); 
printf("max_score = %d\n", max_score); 

//  for (i=0; i <= seq2_length; i++) { 
//    for (j=0; j <= seq1_length; j++) { 
////        printf("score_matrix[%d][%d]  %d\n", i,j, score_matrix[i][j] ); 
//    }
//  } 


print_score_matrix(&score_matrix[0][0],  seq2_length+1, seq1_length+1  ); 
print_score_matrix(&direction_matrix[0][0],  seq2_length+1, seq1_length+1  ); 
printf("finished printing score matrix= %d\n", max_score); 
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
printf("align1_index = %d, align2_index = %d\n", align1_index, align2_index); 
char *align1 ; 
char *align2; 
align1 = (char *) calloc(seq1_length,sizeof(char) )  ; 
align2 = (char *) calloc(seq2_length,sizeof(char) )  ; 
for(i=0;i<align1_index;i++){ 
  align1[align1_index-i-1] =  alphabet[align1_arr[i]] ; 
} 

printf("align1 = %s\n", align1); 
printf("align2 = %s\n", align1); 


/*
$align1 = reverse $align1;
$align2 = reverse $align2;
print "$align1\n";
print "$align2\n"

*/
return 1; 
}

