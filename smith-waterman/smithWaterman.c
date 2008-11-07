/*-------------------------------------------------------------

Copyright (C) 2000 Peter Clote. 
All Rights Reserved.

Permission to use, copy, modify, and distribute this
software and its documentation for NON-COMMERCIAL purposes
and without fee is hereby granted provided that this
copyright notice appears in all copies.


THE AUTHOR AND PUBLISHER MAKE NO REPRESENTATIONS OR
WARRANTIES ABOUT THE SUITABILITY OF THE SOFTWARE, EITHER
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
PARTICULAR PURPOSE, OR NON-INFRINGEMENT. THE AUTHORS
AND PUBLISHER SHALL NOT BE LIABLE FOR ANY DAMAGES SUFFERED
BY LICENSEE AS A RESULT OF USING, MODIFYING OR DISTRIBUTING
THIS SOFTWARE OR ITS DERIVATIVES.

-------------------------------------------------------------*/


/*************************************************
	Program: smithWaterman.c
	Peter Clote, 11 Oct 2000

Program for local sequence alignment, using the Smith-Waterman
algorithm and assuming a LINEAR gap penalty.
A traceback is used to determine the alignment, and
is determined by choosing that direction, for
which S(i-1,j-1)+sigma(a_i,b_j), S(i-1,j)+Delta and 
S(i,j-1)+Delta is maximum, i.e.  for which 

                    _
                   |
                   | H(i-1,j-1) + sigma(a_i,b_j)  (diagonal)
H(i,j) =  MAX of   | H(i-1,j)   + delta           (up)
                   | H(i,j-1)   + delta           (left)
                   | 0
                   | _


is a maximum.

*************************************************/


#include <stdio.h>
#include <ctype.h>   	// character handling
#include <stdlib.h>     // def of RAND_MAX 
#define N 1000         // size of protein arrays
#define AA 20           // number of amino acids
#define MAX2(x,y)     ((x)<(y) ? (y) : (x))
#define MAX3(x,y,z)   (MAX2(x,y)<(z) ? (z) : MAX2(x,y))


	void error(char *);		/** error handling */
	int char2AA(char);
	char AA2char(int);

int main(int argc, char *argv[]) {
	// function prototypes

	// variable declarations
	FILE * in1, *in2, *pam;
	char ch;
	int temp;
	int i,j,tempi,tempj,x,y,diag,down,right,DELTA;
	int topskip,bottomskip;
	char aout[N],bout[N];
	int Aend,Bend,Abegin,Bbegin;
	int max, Max, xMax, yMax;	
		// Max is first found maximum in similarity matrix H
		// max is auxilliary to determine largest of
		// diag,down,right, xMax,yMax are h-coord of Max
	short a[N], b[N];
	int h[N][N];
	int sim[AA][AA];		// PAM similarity matrix
	short xTraceback[N][N], yTraceback[N][N];

	/**** Error handling for input file ****/	
	if (argc != 5) {
	     	fprintf(stderr,"%s protein1 protein2 PAM gapPenalty\n",argv[0]);
		exit(1);
		}

	/***** Initialization of input file and pair array **/
	in1 = fopen(argv[1],"r");
	in2 = fopen(argv[2],"r");
	pam = fopen(argv[3],"r");
	DELTA = atoi(argv[4]);

	/** read PAM250 similarity matrix **/	
	for (i=0;i<AA;i++)
		for (j=0;j<=i;j++) {
		if (fscanf(pam, "%d ", &temp) == EOF) {
			fprintf(stderr, "PAM file empty\n");
			fclose(pam);
			exit(1);
			}
		sim[i][j]=temp;
		}
	fclose(pam);
	for (i=0;i<AA;i++)
		for (j=i+1;j<AA;j++) 
			sim[i][j]=sim[j][i]; 	// symmetrify



	/** read first file in array "a" **/	
	i=0;
	a[0]=0;  // bogus first character so indices go from 1...n

	do {
		ch=fgetc(in1);
	} while (ch != 10 && ch != 13);	
		// 10 is new line, 13 is carriage return
		// hence skip first line
	
	do {
		ch = fgetc(in1);
		if ( ch == EOF ) break;
		LOOP1: if ( ch == 10 || ch == 13 ) {
			// newline or carriage return
			ch = fgetc(in1);
			if ( ch == EOF ) break;
			goto LOOP1;
			}
		// Bad style with 'goto', but then 
		// one need not assume there aren't 2 '\n' in a row  

		while ( isdigit(ch)|| ch==' ' ) ch=fgetc(in1); 
		// skip over position number and blanks

		a[++i]= char2AA(ch);	// preincrement and cast
		if (i+1==N) break;	// don't overflow array size

	} while ( 1 );		/** infinite while loop */
	a[0]=i;			// number of entries in "a" array
	fclose(in1);

	/** read first file in array "b" **/	
	i=0;
	b[0]=0;  // bogus first character so indices go from 1...n

	do {
		ch=fgetc(in2);
	} while (ch != 10 && ch != 13);	
		// 10 is new line, 13 is carriage return
		// hence skip first line

	do {
		ch = fgetc(in2);
		if ( ch == EOF ) break;
		LOOP2: if ( ch == 10 || ch == 13 ) {
			// newline or carriage return
			ch = fgetc(in2);
			if ( ch == EOF ) break;
			goto LOOP2;
			}
		// Bad style with 'goto', but then 
		// one need not assume there aren't 2 '\n' in a row  

		while ( isdigit(ch)|| ch==' ' ) ch=fgetc(in2); 
		// skip over position number and blanks

		b[++i]= char2AA(ch);	// preincrement and cast
		if (i+1==N) break;	// don't overflow array size
	} while ( 1 );		/** infinite while loop */
	b[0]=i;			// number of entries in "b" array
	fclose(in2);


	/** initialize traceback array **/
	Max=xMax=yMax=0;
	for (i=0;i<=a[0];i++)
		for (j=0;j<=b[0];j++) {
			xTraceback[i][j]=-1;
			yTraceback[i][j]=-1;
			}

	/** compute "h" local similarity array **/
	for (i=0;i<=a[0];i++) h[i][0]=0;
	for (j=0;j<=b[0];j++) h[0][j]=0;
	
	for (i=1;i<=a[0];i++)
		for (j=1;j<=b[0];j++) {
			diag    = h[i-1][j-1] + sim[a[i]][b[j]];
			down    = h[i-1][j] + DELTA;
			right   = h[i][j-1] + DELTA;
			max=MAX3(diag,down,right);
			if (max <= 0)  {
				h[i][j]=0;
				xTraceback[i][j]=-1;
				yTraceback[i][j]=-1;
					// these values already -1
				}
			else if (max == diag) {
				h[i][j]=diag;
				xTraceback[i][j]=i-1;
				yTraceback[i][j]=j-1;
				}
			else if (max == down) {
				h[i][j]=down;
				xTraceback[i][j]=i-1;
				yTraceback[i][j]=j;
				}
			else  {
				h[i][j]=right;
				xTraceback[i][j]=i;
				yTraceback[i][j]=j-1;
				}
			if (max > Max){
				Max=max;
				xMax=i;
				yMax=j;
				}
			}  // end for loop


	// output values for gnuplot
	i=xMax; j=yMax;
	while ( i>0 && j>0 && h[i][j]>0){
		printf("%d %d\n",i,j);
		//printf("%c %c\n",AA2char(a[i]),AA2char(b[j]));
		//printf("score %d\n",h[i][j]);
			// previous 2 lines for debugging

		tempi=i;
		tempj=j;
		i=xTraceback[tempi][tempj];
		j=yTraceback[tempi][tempj];
			// WARNING -- following 2 lines incorrect!
			// You need tempi, tempj
			//i=xTraceback[i][j];
			//j=yTraceback[i][j];
	}




	// initialize output arrays to be empty -- this is unnecessary
	for (i=0;i<N;i++) aout[i]=' ';
	for (i=0;i<N;i++) bout[i]=' ';
	

	// reset to max point to do alignment
	i=xMax; j=yMax;
	x=y=0;
	topskip = bottomskip = 1;
	while (i>0 && j>0 && h[i][j] > 0){
		if (topskip && bottomskip) {
			aout[x++]=AA2char(a[i]);
			bout[y++]=AA2char(b[j]);
			}
		else if (topskip) {
			aout[x++]='-';
			bout[y++]=AA2char(b[j]);
			}
		else if (bottomskip) {
			aout[x++]=AA2char(a[i]);
			bout[y++]='-';
			}
		topskip    = (j>yTraceback[i][j]);
		bottomskip = (i>xTraceback[i][j]);
		tempi=i;tempj=j;
		i=xTraceback[tempi][tempj];
		j=yTraceback[tempi][tempj];
			// Warning -- following 2 lines no good
			// i=xTraceback[i][j];
			// j=yTraceback[i][j];
	}

	

	// print alignment
	printf("\n");
	printf("\n");
	for (i=x-1;i>=0;i--) printf("%c",aout[i]);	
	printf("\n");
	for (j=y-1;j>=0;j--) printf("%c",bout[j]);	
	printf("\n");
	printf("\n");
	
        return 0; 
}

void error(char * s) {
	fprintf(stderr,"%s\n",s);
	exit(1);
}





int char2AA(char ch){
	switch (ch) {
		case 'C' : return 0;
		case 'G' : return 1;
		case 'P' : return 2;
		case 'S' : return 3;
		case 'A' : return 4;
		case 'T' : return 5;
		case 'D' : return 6;
		case 'E' : return 7;
		case 'N' : return 8;
		case 'Q' : return 9;
		case 'H' : return 10;
		case 'K' : return 11;
		case 'R' : return 12;
		case 'V' : return 13;
		case 'M' : return 14;
		case 'I' : return 15;
		case 'L' : return 16;
		case 'F' : return 17;
		case 'Y' : return 18;
		case 'W' : return 19;
		default  : 
			fprintf(stderr,"Nonstandard amino acid code: %d\n",ch);
			exit(1);
		}
}


char AA2char(int x){
	switch (x) {
		case 0 : return 'C';
		case 1 : return 'G';
		case 2 : return 'P';
		case 3 : return 'S';
		case 4 : return 'A';
		case 5 : return 'T';
		case 6 : return 'D';
		case 7 : return 'E';
		case 8 : return 'N';
		case 9 : return 'Q';
		case 10: return 'H';
		case 11: return 'K';
		case 12: return 'R';
		case 13: return 'V';
		case 14: return 'M';
		case 15: return 'I';
		case 16: return 'L';
		case 17: return 'F';
		case 18: return 'Y';
		case 19: return 'W';
		default  : fprintf(stderr,"Bad char: %d\n",x);
                           error("Bad integer representation of AA");
                           return ' '; 
		}
}


/*---------------------------------------------------
Output of
	a.out Cdc25 Ste5 pam250 -10


999 821
998 820
997 819
996 818
995 817
994 816
993 815
992 814
991 813
990 812
989 811
988 810
987 809
986 808
985 807
984 806
983 805
982 804
981 803
980 802
979 801
978 800
977 799
976 798
975 797
974 796
973 795
972 794
971 793
970 792
969 791
968 790
967 789
966 788
965 787
964 786
963 785
962 784
961 783
960 782
959 781
958 780
957 779
956 778
955 777
954 776
953 775
952 774
951 773
950 772
949 771
948 770
947 769
946 768
945 767
944 766
943 765
943 764
942 763
941 762
941 761
940 760
939 759
938 758
937 757
936 756
935 755
934 754
933 753
932 752
931 751
930 750
929 749
928 748
927 747
926 746
925 745
924 744
923 743
922 742
921 741
920 740
919 739
918 738
917 737
916 736
915 735
914 734
913 733
912 732
911 731
910 730
909 729
908 728
908 727
907 726
906 725
905 724
904 723
903 722
902 721
901 720
900 719
899 718
898 717
897 716
896 715
895 714
894 713
893 712
892 711
891 710
890 709
889 708
888 707
887 706
886 705
885 704
884 703
883 702
882 701
881 700
880 699
879 698
878 697
877 696
876 695
875 694
874 693
873 692
872 691
871 690
870 689
869 688
868 687
867 686
866 685
865 684
864 683
863 682
862 681
861 680
860 679
859 678
858 677
857 676
856 675
855 674
854 673
853 672
853 671
852 670
851 669
850 668
849 667
848 666
847 665
846 664
845 663
844 662
843 661
842 660
841 659
840 658
839 657
838 656


EHLKIISKPKSRIRN-LEINSSTYEQINQNVLLLEILENLDLSIFINLKNLIKTPSILLDLESEEFLVHAM-SSVSSVLTEFFDIKQAFHDIVIRLIMTTQQTTL-DD-PYLFSSMRSNFPVGHHEPFKNISNTPLVKGPFHKKNEQLALSLFHVLVSQDVEFNNL
DELVLLLPPREIAKQLCILEFQSFSHISRIQFLTKIWDNLNRFSPKEKTSTFYLSNHLVNFVTETIVQEEEPRRRTNVLAYFIQVCDYLRELNNFASLFSIISALNSSPIHRLRKTWANLNSKTLASFELLNNLTEARKNFSNYRDCLENCVLPCVPFLGVYFTDL

---------------------------------------------------*/

