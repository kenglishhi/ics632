main: clean  
#	gcc -Wall sw_test.c  -o sw_test
#	gcc -Wall sw_sequential.c  -o sw_sequential
#	gcc -Wall sw_seq_lib.c sw_sequential_random.c  -o sw_sequential_random
#	gcc -Wall sw_simple.c  -o sw_simple
#	gcc -Wall sw_basic.c  -o sw_basic
#	gcc -Wall sw_small.c  -o sw_small
	mpicc -Wall sw_2d_lib.c 2d_test.c -o 2d_test -lm
	mpicc -Wall sw_2d_lib.c 2d_test2.c -o 2d_test2 -lm
	mpicc -Wall sw_2d_lib.c 2d_test3.c -o 2d_test3 -lm
	mpicc -Wall sw_2d_lib.c sw_2d.c -o sw_2d -lm
clean:
#	/bin/rm -f sw_test sw_sequential sw_simple
#	/bin/rm -f sw_basic sw_small sw  sw_parallel
	/bin/rm -f q 2d_test 2d_test2 2d_test3 sw_2d 2d_test2  2d_test3  sw_2d     

