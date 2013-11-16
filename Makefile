CC=gcc
CCFLAGS=-ggdb

all: gen

uniform.o: uniform.c
	$(CC) -c $(CCFLAGS) uniform.c
	
expon.o: expon.c
	$(CC) -c $(CCFLAGS) expon.c

topology_generator.o: topology_generator.c
	${CC} -c ${CCFLAGS} topology_generator.c
	
gen: topology_generator.o uniform.o expon.o
	${CC} -o gen topology_generator.o uniform.o expon.o -lm
	
clean:
	rm -rf *.o gen