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
#include "random.h"
#include "queue.h"

//powerlaw globals
double seed = 123456789;
double alpha = -2.5;
int min = 1;
int max = 10;

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

		for(int i = nodes + 1; i < size; i++)
		{
			graph[i] = (int *)malloc((max+2)*(sizeof(int)));
			graph[i][0] = i - 1;
			eligible[i] = 1;

			for(int j = 1; j < max + 2; j++)
				graph[i][j] = -1;
		}
	}
}
int duplicate(int node, int canidate)
{
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
		nodes++;    //note that you are adding a new node
		pairTo = nodes;  //choose the new node
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

	//add connection to prior node to avoid partition
	for(int i = 1; i < nodes; i++)
		graph[i][0] = i -1;

	//initialize to -1 so you can find end of connection list
	for(int i = 0; i < nodes; i++)
	{
		for(int j = 1; j < max + 2; j++)
			graph[i][j] = -1;
	}	

	fp = fopen("graph.txt", "w");
}

void writeOutput()
{
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
}
void cleanup()
{
	//close the file
	fclose(fp);

	//free the adjacency list
	for(int i = 0; i < size; i++)
		free(graph[i]);
	free(graph);

	//free the eligible array
	free(eligible);
}
//arg1: number of nodes to add
int main(int argc, char **argv)
{
	nodes = atoi(argv[1]);
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

	writeOutput();

	cleanup();

	return EXIT_SUCCESS;
}
