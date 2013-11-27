CC=gcc
CCFLAGS=-ggdb

all: test gen mul mgen

uniform.o: uniform.c
	$(CC) -c $(CCFLAGS) uniform.c
	
expon.o: expon.c
	$(CC) -c $(CCFLAGS) expon.c

power.o: power.c
	$(CC) -c $(CCFLAGS) power.c

test.o: test.c
	$(CC) -c $(CCFLAGS) test.c

test: test.o uniform.o power.o
	$(CC) -o test test.o uniform.o power.o -lm

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
	
mgen: matrix-topology.o
	${CC} -o mgen matrix-topology.o uniform.o expon.o -lm
	
clean:
	rm -rf *.o gen mul test
