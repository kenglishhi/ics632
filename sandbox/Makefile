my_executable: main.o func.o
	gcc main.o func.o -o my_executable

main.o: main.c
	gcc -c main.c -o main.o

func.o: func.c
	gcc -c func.c -o func.o

clean:
	/bin/rm -f main.o func.o my_executable
