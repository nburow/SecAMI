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
int size = 100;  //MAX size of strings
int total = 0;   //total number of nodes you have created
int *parent;

char *addParent(int skippedNode, int node, char *s)
{
	//string to return
	char *p = malloc(size*sizeof(char));
	memset(p, 0, size*sizeof(char));

	//temporary string, used for formatting (sprintf overwrites existing string);
	char *tmp = malloc(size*sizeof(char));
	memset(tmp, 0, size*sizeof(char));

	if(skippedNode < node)
	{
		//add those nodes only connected to their parent
		int i;
		for(i = skippedNode; i < node; i++)
		{
			sprintf(tmp, "%d: %d\n", i, parent[i]);
			strcat(p, tmp);
		}

		//prepend those nodes to the entry for the node with new connections
		//done by copying s to the end of p;
		char *middle = p + strlen(p);
		if((strlen(p) + strlen(s)) > size - 2)
		{
			size *= 2;
			realloc(p, size*sizeof(char));
		}
		strcpy(middle, s);
		free(s);

		sprintf(tmp, "%d", parent[node]);
		strcat(p, tmp);
	}
	else
	{
		//move s into p so can return the same thing regardless of case
		strcpy(p, s);
		free(s);

		//add the current node's parent
		sprintf(tmp, "%d", parent[node]);
		strcat(p, tmp);
	}
	free(tmp);
	return p;
}
//add connections between nodes at the same level
char *addLateral(int min, int max)
{
	return NULL;
}
//add new nodes
char *addNewNodes(int root, int num)
{
	char *line = malloc(size*sizeof(char));
	char tmp[10];
	memset(line, 0, size*sizeof(char));
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
	int resetPoint = 0;     //in the event no new nodes get added, but you still need
	                       //more nodes, circle back to here to try again

	char *entry = malloc(size*sizeof(char));
	memset(entry, 0, sizeof(size*sizeof(char)));

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
			//node 0 has no parent...
			if(nodeNum > 0)
				entry = addParent(resetPoint, nodeNum, entry);

			//append list of new connections to current node
			char *newNodes = addNewNodes(nodeNum, numNewNodes);
			strcat(entry, newNodes);
			free(newNodes);

			nodeNum++;              //move to next node
			resetPoint = nodeNum;     //once some node has added new nodes, only add to nodes after it
			fprintf(fp, "%s", entry);
		}
		else if (nodeNum < total)
			nodeNum++;               //added no connections, but move on to next node
		else
			nodeNum  = resetPoint;    //none of the available nodes had connections added, reset
	}
	free(entry);
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
