#include <stdlib.h>
#include <stdio.h>
#include "random.h"
#include "bfs.h"

int main(int argc, char **argv)
{
	FILE *in = fopen("graph.txt", "r");
	int **g = getGraph(in);
	//myprint(g);
	int start = atoi(argv[1]);
	int end = atoi(argv[2]);
	int x = distance(start, end, g);
	printf("dist from %d to %d is %d\n", start, end, x);
}
