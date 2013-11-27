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

int *eligible;

//adjacency list
int **graph;
int nodes;

void addNodes(int node, int connections)
{
	//find the first slot that needs a connection for the node
	int i = 0;
	while(graph[node][i] != -1)
		i++;

	while(connections > 0)
	{
		int pairTo = uniform(&seed)*nodes; //select a node at random
		while(eligible[pairTo] == 0)
			pairTo = uniform(&seed)*nodes;
		int j = 0;
		while(graph[pairTo][j] != -1)
			j++;
		
		//add the mutual connections
		graph[node][i] = pairTo;
		i++;
		graph[pairTo][j] = node;
		
		connections--;
	}
}

//creates the adjacency list
void init()
{
	graph = (int **)malloc(nodes*(sizeof(int *)));
	for(int i = 0; i < nodes; i++)
	{
		graph[i] = (int *)malloc((max+1)*(sizeof(int)));
	}
	
	//add connection to prior node to avoid partition
	for(int i = 1; i < nodes; i++)
		graph[i][0] = i -1;
	
	//initialize to -1 so you can find end of connection list
	for(int i = 0; i < nodes; i++)
	{
		for(int j = 1; j < max + 1; j++)
			graph[i][j] = -1;
	}	
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
		eligible[i] = (int) round( power_rng(&seed, alpha, min, max) );
	
	//add those connnections as necessary
	for(int i = 0; i < nodes; i++)
	{
		if(eligible[i] > 0)
			addNodes(i, eligible[i]);
	}

	return EXIT_SUCCESS;
}
