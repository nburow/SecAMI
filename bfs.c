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
#include <limits.h>
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
	//printf("next line: %s\n", line);

	int i = 0;
	while((strlen(line) > 0))
	{
		word = parseWord(&line);

		if(i >= size - 1)
		{
			//printf("realloc cols\n");
			size *= 2;
			retval = (int **)realloc(retval, size*sizeof(int *));
			for(int k = i+1; k < size; k++)
				retval[k] = (int *)malloc(len*sizeof(int));
			for(int k = i+1; k < size; k++)
			{
				for(int l = 0; l < len; l++)
					retval[k][l] = -1;
			}
		}

		int j = 0;
		while((strlen(word) > 0))
		{
			//if(i == 1)
			//printf("next word: %s\n", word);
			//printf("j %d len %d\n", j, len);
			if(j >= len -1)  //leave room for terminating -1
			{
				//printf("realloc rows\n");
				len *= 2;
				for(int k = 0; k < size; k++)
				{
					retval[k] = (int *)realloc(retval[k], len*sizeof(int));
					for(int l = len/2; l < len; l++)
						retval[k][l] = -1;
				}
				/*int k = 0;
				while(retval[j][k] != -1)
					printf("%d\t", retval[j][k++]);
				printf("\n");*/
			}
			int node = atoi(word);
			//if(i == 1)
			//printf("adding: %d\n", node);
			retval[i][j] = node;
			word = parseWord(&line);
			j++;
		}
		//printf("last entry in line %d: %d\n", i, retval[i][j]);
		line = getLine(in);
		//printf("next line: %s\n", line);
		i++;
	}
	return retval;
}

int *bfs(int start, int **graph)
{
	int *bfs = (int *)malloc(nodes*sizeof(int));
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
	return bfs;
}

int dijkstra(int start, int end, int **graph)
{
	int unvisited[nodes];   //1 if unvisited, 0 if visited
	int distance[nodes];   //distance to node x
	for(int i = 0; i < nodes; i++)
	{
		unvisited[i] = 1;
		distance[i] = INT_MAX;
	}
	int current = start;
	distance[current] = 0;

	while(current != end)
	{
		//printf("current: %d distance: %d\n", current, distance[current]);
		int i = 1;
		int x = graph[current][i];
		while(x != -1)
		{
			if(distance[current] + 1 < distance[x])
			{
				distance[x] = distance[current] + 1;
				//printf("new distance for %d is %d\n", x, distance[x]);
			}
			i++;
			x = graph[current][i];
		}
		unvisited[current] = 0;
		int min = INT_MAX;
		int next = -1;
		for(int j = 0; j < nodes; j++)
		{
			if(unvisited[j] == 1 && distance[j] < min)
			{
				min = distance[j];
				next = j;
			}
		}
		current = next;
		//printf("\n");
		//printf("just visited: %d distance: %d \n", current, distance[current]);
	}
	//printf("\n\n");
	return distance[end];
}

void bfsPrint(int *bfs)
{
	for(int i = 0; i < nodes; i++)
		printf("%d ", bfs[i]);
	printf("\n");
}

void myprint(FILE* fp, int** graph)
{
	fprintf(fp, "nodes: %d\n", nodes);
	for(int i = 0; i < size; i++)
	{
		if(graph[i][0] == -1)
			break;
		for(int j = 0; j < len; j++)
		{
			if(graph[i][j] != -1 && graph[i][j] != -2)
				fprintf(fp, "%d ", graph[i][j]);
			else if(graph[i][j] == -2)
				continue;
			else
				break;
		}
		fprintf(fp, "\n");
	}
}
/*
void bfsPrint(int *bfs)
{
	for(int i = 0; i < nodes; i++)
		printf("%d ", bfs[i]);
	printf("\n");
}
 */
//arguments: name of input file.
/*int main(int argc, char** argv)
{
	FILE *in = fopen(argv[1], "r");
	int from = atoi(argv[2]);
	int to = atoi(argv[3]);

	int **graph = getGraph(in);
	//myprint();
	for(int j = 0; j < nodes; j++)
	{
		int i = 1;
		printf("%d: ", j);
		while(graph[j][i] != -1)
			printf("%d, ", graph[j][i++]);
		printf("\n");
	}

	for(int i = 0; i < nodes; i++)
	{
		int x = dijkstra(0, i, graph);
		int y = dijkstra(i, 0, graph);
		if(x != y)
			printf("problem with %d\n", i);
	}
	//printf("distance %d\n",dijkstra(from, to, graph));

	fclose(in);
	return EXIT_SUCCESS;
}*/
//*/

