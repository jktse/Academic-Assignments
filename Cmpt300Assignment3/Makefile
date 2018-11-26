run: list.o main.o
	gcc -std=c99 -o run list.o main.o

list.o: list.c list.h node.h
	gcc -c list.c

main.o: main.c list.h
	gcc -c main.c

clean:
	rm -f run *.o


