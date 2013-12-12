/*
 * matrix-topology.c
 *
 *  Created on: Nov 20, 2013
 *      Author: nburow
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "random.h"
#include "queue.h"

//powerlaw globals
double seed;
double alpha = -2.5;
int min = 1;
int max;

int *eligible;         //array with the number of connections per node

//adjacency list
int **graph;           //pointer to matrix
int nodes;             //number of nodes currently existent
int size;              //number of rows currently possible

//file to write final list to
FILE *fp;

void resize()
{
	printf("resizing\n");
	if(nodes+1 > size)
	{
		size *=2;
		graph = (int **)realloc(graph, size*sizeof(int *));
		eligible = (int *)realloc(eligible, size*sizeof(int));
		printf("realloc\n");

		for(int i = nodes; i < size; i++)
		{
			graph[i] = (int *)malloc((max+2)*(sizeof(int)));
			eligible[i] = 1;

			for(int j = 0; j < max + 2; j++)
				graph[i][j] = -1;
		}
	}
}
int duplicate(int node, int canidate)
{
	if(node == canidate)
		return 1;
	int i = 0;
	while(graph[node][i] != -1)
	{
		if(graph[node][i] == canidate)
			return 1;  //found a duplicate, return true
		i++;
	}
	return 0;
}

//returns index of a node that still needs connections
int findNode(int node)
{
	int pairTo = uniform(&seed)*nodes; //select a node at random
	int count = 0;

	//if duplicate or no connections and have try's left, try again
	while(count < nodes && (duplicate(node, pairTo) || eligible[pairTo] == 0))
	{
		pairTo = uniform(&seed)*nodes;
		count++;
	}
	//if no one has connections left, add a new node
	if(count == nodes)
	{
		resize();   //make the graph array bigger if necessary
		pairTo = nodes;  //choose the new node
		nodes++;    //note that you are adding a new node
	}
	return pairTo;
}

//adds connections number of connections to node node
void addConnections(int node, int connections)
{
	//find the first slot that needs a connection for the node
	int i = 0;
	while(graph[node][i] != -1)
		i++;

	while(connections > 0)
	{
		int pairTo = findNode(node);
		int j = 0;
		while(graph[pairTo][j] != -1)
			j++;

		//add the mutual connections
		graph[node][i] = pairTo;
		i++;
		graph[pairTo][j] = node;

		eligible[pairTo]--;
		connections--;
	}
	eligible[i] = 0;
}

//creates the adjacency list, opens a file pointer
void init()
{
	size = nodes;
	graph = (int **)malloc(nodes*(sizeof(int *)));
	for(int i = 0; i < nodes; i++)
	{
		//max+1 to leave room for our extra connection to avoid partitions
		graph[i] = (int *)malloc((max+2)*(sizeof(int)));
	}

	//connected to node 1;
	graph[0][0] = 1;
	//add connection to prior node (two way) to avoid partition
	for(int i = 1; i < nodes - 1; i++)
	{
		graph[i][0] = i -1;
		graph[i][1] = i + 1;
	}
	graph[nodes -1][0] = nodes - 2;
	graph[nodes -1][1] = -1;
	graph[0][1] = -1;
	//initialize to -1 so you can find end of connection list
	for(int i = 0; i < nodes; i++)
	{
		for(int j = 2; j < max + 2; j++)
			graph[i][j] = -1;
	}	


}

void writeOutput(char *fileName)
{
	fp = fopen(fileName, "w");
	if (fp == NULL)
	{
		printf("%s is a bad file\n", fileName);
		exit(EXIT_FAILURE);
	}
	//printf("nodes: %d\n", nodes);
	fprintf(fp, "%d\n", nodes);
	for(int i = 0; i < nodes; i++)
	{
		fprintf(fp, "%d:", i);
		int j = 0;
		while(graph[i][j] != -1)
		{
			fprintf(fp, " %d,", graph[i][j]);
			j++;
		}
		fprintf(fp, "\n");
	}
	//close the file
	fclose(fp);
}

void cleanup()
{
	//free the adjacency list
	for(int i = 0; i < size; i++)
		free(graph[i]);
	free(graph);

	//free the eligible array
	free(eligible);
}

void makeGraph(int num)
{
	eligible = (int *)malloc(nodes*sizeof(int));
	memset(eligible, 0, nodes*sizeof(int));

	init();

	//determine how many additional connections each node needs
	for(int i = 0; i < nodes; i++)
		eligible[i] = (int) round(power_rng(&seed, alpha, min, max));

	//add those connections as necessary
	for(int i = 0; i < nodes; i++)
	{
		if(eligible[i] > 0)
			addConnections(i, eligible[i]);
	}
	char *base = "Graphs/graph";
	char fileNumber[3];
	sprintf(fileNumber, "%d", num);
	char *extension = ".txt";
	int fileNameLength = strlen(base) + strlen(fileNumber) + strlen(extension) + 1;

	char fileName[fileNameLength];
	memset(fileName, 0, fileNameLength*sizeof(char));

	strcat(fileName, base);
	strcat(fileName, fileNumber);
	strcat(fileName, extension);

	writeOutput(fileName);

	cleanup();
}
//arg1: number of nodes to add; arg2: max connections / node;
//arg3: 9 digit seed for RNG; argv4: number of graphs to make
int main(int argc, char **argv)
{
	nodes = atoi(argv[1]);
	max = atoi(argv[2]);
	seed = atof(argv[3]);
	int numGraphs = atoi(argv[4]);

	if(mkdir("Graphs", 0777) == -1)
		printf("bad directory\n");

	numGraphs = (numGraphs > 999) ? 999 : numGraphs;

	for(int i = 0; i < numGraphs; i++)
		makeGraph(i);

	return EXIT_SUCCESS;
}
