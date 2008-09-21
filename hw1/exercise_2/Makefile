main: clean timediff lgsearch
	gcc -Wall lgsearch1.c  ../lib/mystery_function_800.o ../lib/timediff.o lgsearch.o  -lm -o ~/bin/lgsearch1.exe 
	gcc -Wall -fopenmp lgsearch2.c  ../lib/mystery_function_800.o ../lib/timediff.o lgsearch.o  -lm -o ~/bin/lgsearch2.exe 
	gcc -Wall -fopenmp lgsearch3.c  ../lib/mystery_function_800.o ../lib/timediff.o lgsearch.o  -lm -o ~/bin/lgsearch3.exe 
clean:
	/bin/rm -f ~/bin/lgsearch*.exe  ../lib/timediff.o lgsearch.o 
timediff:
	gcc -c ../lib/timediff.c  -o ../lib/timediff.o
lgsearch:
	gcc -c lgsearch.c
