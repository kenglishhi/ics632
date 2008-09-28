main: clean timediff 
	gcc -Wall jacobi_sequential.c  ../lib/timediff.o  -lm -o ~/bin/jacobi_sequential.exe 
	gcc -Wall gauss_seidel_sequential.c   ../lib/timediff.o  -lm -o ~/bin/gauss_seidel_sequential.exe 
	gcc -Wall test.c  -o ~/bin/test2600.exe 
clean:
	/bin/rm -f ~/bin/jacobi*.exe  ../lib/timediff.o  
timediff:
	gcc -c ../lib/timediff.c  -o ../lib/timediff.o
