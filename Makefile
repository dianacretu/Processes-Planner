build: planificator.o
	gcc -Wall -g planificator.o -o planificator

planificator.o: Proces.h
	gcc -Wall -c planificator.c

clean:
	rm -f planificator planificator.o
