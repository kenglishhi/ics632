main: clean  
#	mpicc -Wall sw_2d_cyclic_lib.c sw_2d_cyclic.c -o sw_2d_cyclic -lm
	mpicc -Wall 2d_test1.c sw_2d_cyclic_lib.c -o 2d_test1 -lm
clean:
#	/bin/rm -f sw_test sw_sequential sw_simple
#	/bin/rm -f sw_basic sw_small sw  sw_parallel
	/bin/rm -f  sw_2d_cyclic 2d_test1
	/bin/rm -f *.o

