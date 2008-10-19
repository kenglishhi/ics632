

#GCC = /home/casanova/public/bin/gcc
GCC = gcc


main: clean timediff lgsearch
	$(GCC) -Wall  doublecompare.c  -lm -o ~/bin/doublecompare.exe 
	$(GCC) -Wall  lgsearch1.c  ../lib/mystery_function_800_v2.o ../lib/timediff.o lgsearch.o  -lm -o ~/bin/lgsearch1.exe 
	$(GCC) -Wall -fopenmp  lgsearch2.c ../lib/mystery_function_800_v2.o ../lib/timediff.o lgsearch.o  /home/casanova/PACKAGES/lib/libgomp.a -lpthread -lm  -o ~/bin/lgsearch2.exe
	$(GCC) -Wall -fopenmp lgsearch3.c ../lib/mystery_function_800_v2.o ../lib/timediff.o lgsearch.o /home/casanova/PACKAGES/lib/libgomp.a -lpthread  -lm  -o ~/bin/lgsearch3.exe
clean:
	/bin/rm -f ~/bin/lgsearch*.exe  ../lib/timediff.o lgsearch.o 
timediff:
	$(GCC) -c ../lib/timediff.c  -o ../lib/timediff.o
lgsearch:
	$(GCC) -c lgsearch.c
