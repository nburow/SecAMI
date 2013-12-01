#include <stdlib.h>
#include <stdio.h>
#include "bfs.h"
//#include "random.h"

void bfsPrint(int *bfs)
{
	int i = 0;
	while(bfs[i] != -1)
		printf("%d ", bfs[i++]);
	printf("\n");
}

int main()
{
	FILE *in = fopen("graph.txt", "r");
	int **g = getGraph(in);
	int *b = bfs(1, g);

	//myprint(g);
	bfsPrint(b);
}
