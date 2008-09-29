main: clean timediff 
	gcc -Wall -O0 jacobi.c  ../lib/timediff.o  -lm -o ~/bin/jacobi_sequential.exe 
	gcc -Wall gseidel.c   ../lib/timediff.o  -lm -o ~/bin/gseidel_sequential.exe 
	gcc -Wall -fopenmp jacobi_parallel.c   ../lib/timediff.o  -lm -o ~/bin/jacobi_parallel.exe 
	gcc -Wall -fopenmp gseidel_parallel.c   ../lib/timediff.o  -lm -o ~/bin/gseidel_parallel.exe 
clean:
	/bin/rm -f ~/bin/jacobi*.exe ~/bin/gseidel*.exe   ../lib/timediff.o  
timediff:
	gcc -c ../lib/timediff.c  -o ../lib/timediff.o
