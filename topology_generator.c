/*
 * topology_generator.c
 *
 *  Created on: Nov 14, 2013
 *      Author: nburow
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "random.h"

double seed = 123456789;
int total = 0;   //total number of nodes you have created
int *parent;

void addParent(int node, char *s)
{
	char p[10];
	memset(p, 0, sizeof(p));
	sprintf(p, "%d", parent[node]);
	strcat(s, p);
}
//add connections between nodes at the same level
char *addLateral(int min, int max)
{
	return NULL;
}
//add new nodes
char *addNewNodes(int root, int num)
{
	char *line = malloc(1000*sizeof(char));
	char tmp[10];
	memset(line, 0, 1000*sizeof(char));
	memset(tmp, 0, sizeof(tmp));

	int i;
	for(i = 0; i < num; i++)
	{
		if(root == 0 && i == 0)
			sprintf(tmp, "%d", total);
		else
			sprintf(tmp, ", %d", total);
		strcat(line, tmp);

		parent[total] = root;
		total++;                        //added a node so increase the total
	}
	strcat(line, "\n");
	return line;
}

void makeNetwork(int nodes, double mean, FILE *fp)
{

	int nodeNum = 0;           //which node you are currently on
	int numNewNodes = 0;    //number of connections this node has
	int prevTotal = 0;
	int resetPoint = 0;

	char *entry = malloc(1000*sizeof(char));
	memset(entry, 0, sizeof(1000*sizeof(char)));

	while(total < nodes)
	{
		numNewNodes = expon(&seed, mean);

		//make sure we aren't creating too many nodes
		if ((total + numNewNodes) > nodes)
			numNewNodes = nodes - total;

		sprintf(entry, "%d: ", nodeNum);

		if(nodeNum == 0)
			total++;              //account for first node, otherwise counted when added

		if(numNewNodes > 0)
		{
			if(nodeNum > 0)
				addParent(nodeNum, entry);

			//strcat(entry, addPrior(parent[nodeNum], nodeNum, prevTotal));
			char *newNodes = addNewNodes(nodeNum, numNewNodes);
			strcat(entry, newNodes);
			free(newNodes);

			nodeNum++;              //move to next node
			prevTotal = total;
			resetPoint = nodeNum;     //next node is where you want to reset to
			fprintf(fp, entry);
		}
		else if (nodeNum < total)
			nodeNum++;               //added no connections, but move on to next node
		else
			nodeNum  = resetPoint;    //none of the available nodes had connections added, reset
	}
}

//input:  # of nodes, mean # of connections per node
int main(int argc, char** argv)
{
	int nodes = atoi(argv[1]);
	double mean = atof(argv[2]);

	parent = malloc(nodes * sizeof(int));
	memset(parent, 0, nodes*sizeof(int));

	FILE *fp;
	fp = fopen("network.txt", "w");

	makeNetwork(nodes, mean, fp);

	fclose(fp);
	free(parent);
	return EXIT_SUCCESS;
}
