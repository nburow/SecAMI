/*
 * bfs.c
 *
 *  Created on: Nov 30, 2013
 *      Author: nburow
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "queue.h"

//FILE *in;

int size = 100;
int len = 10;

int nodes;

//int **graph;

char *getLine(FILE *in)
{
	char *line = (char *)malloc(size*sizeof(char));
	memset(line, 0, size*sizeof(char));

	char c;
	int i = 0;
	while((c = getc(in)) != EOF && c != '\n')
	{
		line[i++] = c;
		if(i > size - 2) //index: -1, space for '\0' terminator: -2
		{
			size *= 2;
			void *x = realloc(line, size*sizeof(char));
		}
	}
	line[i] = '\0';
	return line;
}
char *parseWord(char **tmp)
{
	char *word = (char *)malloc(size*sizeof(char));
	memset(word, 0, size*sizeof(char));

	int i = 0;
	int j = 0;

	while(isblank((*tmp)[j]) || ispunct((*tmp)[j]))
		j++;

	while(isalnum((*tmp)[j]))
	{
		word[i] = (*tmp)[j];
		i++;
		j++;
	}
	word[i] = '\0';
	*tmp = *tmp + j;

	return word;
}
int **init()
{
	//initialize the array
	int **retval = (int **)malloc(size*sizeof(int *));
	for(int i = 0; i < size; i++)
		retval[i] = (int *)malloc(len*sizeof(int));

	for(int i = 0; i < size; i++)
	{
		for(int j = 0; j < len; j++)
			retval[i][j] = -1;
	}
	return retval;
}

int **getGraph(FILE *in)
{
	int **retval = init();

	char *line = getLine(in);
	char *word = parseWord(&line);
	nodes = atoi(word);

	line = getLine(in);

	int i = 0;
	while((strlen(line) > 0))
	{
		word = parseWord(&line);

		if(i >= size)
		{
			size *= 2;
			retval = (int **)realloc(retval, size*sizeof(int *));
		}

		int j = 0;
		while((strlen(word) > 0))
		{
			if(j >= len)
			{
				len *= 2;
				for(int k = 0; k < size; k++)
				{
					retval[k] = (int *)realloc(retval[k], len*sizeof(int));
					for(int l = len/2; l < len; l++)
						retval[k][l] = -1;
				}
			}
			int node = atoi(word);
			retval[i][j] = node;
			word = parseWord(&line);
			j++;
		}
		line = getLine(in);
		i++;
	}
	return retval;
}

int *bfs(int start, int **graph)
{
	int *bfs = (int *)malloc((nodes+1)*sizeof(int));
	int n = 0;

	int seen[nodes];
	memset(seen, 0, sizeof(seen));

	int root = start;
	seen[root] = 1;
	enqueue(&root, sizeof(root));
	while(!isEmpty())
	{
		int *val = (int *)dequeue();
		for(int i = 1; i < len; i++)
		{
			int x = graph[*val][i];
			if(x == -1)
				break;
			else if(seen[x] == 0)
			{
				enqueue(&x, sizeof(x));
				seen[x] = 1;
			}
		}
		bfs[n++] = *val;
	}
	bfs[n] = -1;
	return bfs;
}

/*
void myprint()
{
	printf("nodes: %d\n", nodes);
	for(int i = 0; i < size; i++)
	{
		if(graph[i][0] == -1)
			break;
		for(int j = 0; j < len; j++)
		{
			if(graph[i][j] != -1)
				printf("%d ", graph[i][j]);
			else
				break;
		}
		printf("\n");
	}
}
void bfsPrint(int *bfs)
{
	for(int i = 0; i < nodes; i++)
		printf("%d ", bfs[i]);
	printf("\n");
}

//arguments: name of input file.
int main(int argc, char** argv)
{
	in = fopen(argv[1], "r");

	graph = getGraph();
	//myprint();

	int start = 4;
	int *bfsList = bfs(start);
	bfsPrint(bfsList);

	fclose(in);
	return EXIT_SUCCESS;
}
*/

