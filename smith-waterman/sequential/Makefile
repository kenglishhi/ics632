main: clean  
#	gcc -Wall sw_test.c  -o sw_test
#	gcc -Wall sw_sequential.c  -o sw_sequential
#	gcc -Wall sw_sequential_old.c -o sw_sequential_old
	gcc -Wall sw_seq_lib.c sw_sequential_random.c  -o sw_sequential_random
	gcc -Wall sw_seq_lib.c sw_sequential.c  -o sw_sequential
#	gcc -Wall sw_simple.c  -o sw_simple
#	gcc -Wall sw_basic.c  -o sw_basic
#	gcc -Wall sw_small.c  -o sw_small
#	mpicc -Wall sw_ring.c  -o sw_ring
#	mpicc -Wall sw_ring_lib.c sw_ring_random.c  -o sw_ring_random
clean:
#	/bin/rm -f sw_test sw_sequential sw_simple
#	/bin/rm -f sw_basic sw_small sw  sw_parallel
	/bin/rm -f sw_sequential  sw_sequential_random sw_sequential_old
