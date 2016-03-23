all: bbst

bbst: EventCounter.o RbTree.o
	gcc -Wall -o bbst EventCounter.o RbTree.o -lm

EventCounter.o: EventCounter.c
	gcc -Wall -c EventCounter.c

RbTree.o: RbTree.c
	gcc -Wall -c RbTree.c

clean:
	rm -rf bbst *.o *~
