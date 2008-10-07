main: clean
	mpicc ../lib/mpi_mybcast.c simple_broadcast.c -o ~/bin/simple_broadcast.exe
	mpicc -Wall ../lib/timediff.c mpi_mybcast.c compare_bcast.c -o ~/bin/compare_bcast.exe
clean:
	/bin/rm -f ~/bin/simple_broadcast.exe  ~/bin/blocking_send.exe -f ~/bin/compare_bcast.exe  
