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

//grid globals
int **grid;  //a grid of locations where a node in the mesh network can reside
int size_x;
int size_y;
int nodes;

//adjacency list
int **graph;

//settings for adding nodes
int range = 10;


void init()
{
	grid = (int **)malloc(size_x*sizeof(int *));
	int i;
	for(i = 0; i < size_x; i++)
		grid[i] = (int *)malloc(size_y*sizeof(int));

	graph = (int **)malloc(nodes*(sizeof(int *)));
	for(i = 0; i < nodes; i++)
		graph[i] = (int *)malloc(max*(sizeof(int)));
}

//arg1: number of nodes to add
//arg2: x dim of grid
//arg3: y dim of grid
int main(int argc, char **argv)
{
	nodes = atoi(argv[1]);
	size_x = atoi(argv[2]);
	size_y = atoi(argv[3]);

	init();

	int total = 0;
	while(total < nodes)
	{
		double x = power_rng(&seed, alpha, min, max);
		x = round(x);
		int newNodes = (int)(x);
		//addNodes(newNodes);
		total += newNodes;
	}

	return EXIT_SUCCESS;
}
