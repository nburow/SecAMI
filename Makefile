CC=gcc
CPP=g++
CCFLAGS=-std=c99 -ggdb

all: graph attackSim

Heap.o: Heap.c
	$(CC) -c $(CCFLAGS) Heap.c

queue.o: queue.c
	$(CC) -c $(CCFLAGS) queue.c

bfs.o: bfs.c
	$(CC) -c $(CCFLAGS) bfs.c
	
#bfs: bfs.o queue.o
#	$(CC) -o bfs bfs.o queue.o -lm

attackSim.o: attackSim.c
	$(CC) -c $(CCFLAGS) attackSim.c

attackSim: attackSim.o Heap.o bfs.o queue.o uniform.o
	$(CC) -o attackSim attackSim.o Heap.o bfs.o queue.o uniform.o -lm

uniform.o: uniform.c
	$(CC) -c $(CCFLAGS) uniform.c
	
expon.o: expon.c
	$(CC) -c $(CCFLAGS) expon.c

power.o: power.c
	$(CC) -c $(CCFLAGS) power.c

test.o: test.c
	$(CC) -c $(CCFLAGS) test.c

test: test.o uniform.o power.o bfs.o queue.o
	$(CC) -o test test.o uniform.o power.o bfs.o queue.o -lm

topology_generator.o: topology_generator.c
	${CC} -c ${CCFLAGS} topology_generator.c
	
gen: topology_generator.o uniform.o expon.o
	${CC} -o gen topology_generator.o uniform.o expon.o -lm
	
mulvalInput.o: mulvalInput.c
	${CC} -c ${CCFLAGS} mulvalInput.c

mul: mulvalInput.o
	${CC} -o mul mulvalInput.o
	
matrix-topology.o: matrix-topology.c
	${CC} -c ${CCFLAGS} matrix-topology.c
	
graph: matrix-topology.o uniform.o power.o queue.o
	${CC} -o graph matrix-topology.o uniform.o power.o queue.o -lm
	
clean:
	rm -rf *.o gen mul test graph attackSim 
