/* sw_small.c -- Test the speed of the standard Smith-Waterman algorithm for * *   comparison with bit-parallel constrained Smith-Waterman algorithm *   http://coen.boisestate.edu/ssmith/BioHW/CompCode/BitParallel/sw_test.txt *
 */


#include  <stdio.h>
#define STRLEN     8
#define   ITER       1
#define GAP   -1
#define MATCH 1
#define MISMATCH 1
#define SEQ1_LEN 8
#define DIRECTION_NONE     0
#define DIRECTION_UP       1
#define DIRECTION_LEFT     2
#define DIRECTION_DIAGONAL 3


void print_matrix(int *matrix, int matrix_size) {
    int *current_value;
    int i,j;

    printf("matrix_size = %d \n\n", matrix_size  );
    for (i = 0; i < matrix_size; i++) {
        printf(" [" );
        for (j = 0; j < matrix_size; j++){
            current_value = matrix + i * matrix_size + j;
            printf(" %d ", *current_value  );
        }
        printf("] " );

        printf("\n" );
    }
}


int  main(int argc,char *argv[]) { 
  char  alphabet[21] = "acdefghiklmnpqrstvwy"; 

  int seq1_length = SEQ1_LEN; 
  int seq2_length = SEQ1_LEN; 
  char seq1[SEQ1_LEN] = "ppeaiccc"; 
  char seq2[SEQ1_LEN] = "ppeaiccc"; 
  int seq1_arr[seq1_length], seq2_arr[seq2_length]  ;
  int i, j; 
  int score_matrix[seq1_length+1][seq2_length+1] ;  
  int direction_matrix[seq1_length+1][seq2_length+1] ;  
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
  for ( i=0; i <= seq2_length; i++ ) { 
    score_matrix[i][0] = 0; 
    direction_matrix[i][0]   = DIRECTION_NONE;
  } 

  for (i=1; i <= seq1_length; i++) { 
    for (j=1; j <= seq2_length; j++) { 

       diagonal_score=0; left_score=0; up_score=0;      
       letter1 = seq1_arr[i-1]; 
       letter2 = seq2_arr[j-1]; 
       // calculate match score 
       if (letter1 == letter2)  
         diagonal_score = score_matrix[i-1][j-1] + MATCH; 
       else 
         diagonal_score = score_matrix[i-1][j-1] + MISMATCH; 
       
       // calculate gap scores
       up_score   = score_matrix[i-1][j] + GAP;
       left_score = score_matrix[i][j-1] + GAP;
          
       if (diagonal_score <= 0 && up_score <= 0 && left_score <= 0) {
            score_matrix[i][j]   = 0;
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


        // set maximum score
        if (score_matrix[i][j] > max_score) {
            max_i     = i;
            max_j     = j;
            max_score = score_matrix[i][j];
        }
    }  
  } 
  print_matrix(score_matrix,  SEQ1_LEN+1 ); 



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
char align1[SEQ1_LEN] ; 
char align2[SEQ1_LEN] ; 
for(i=0;i<align1_index;i++){ 
  align1[align1_index-i-1] =  alphabet[align1_arr[i]] ; 
} 

printf("align1 = %s\n", align1); 


/*
$align1 = reverse $align1;
$align2 = reverse $align2;
print "$align1\n";
print "$align2\n"

*/
      

  
  return 0;
}

