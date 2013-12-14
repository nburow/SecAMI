#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "random.h"
#include "bfs.h"

//flag adjlist
#define END_OF_LIST				-1
#define DISCONNECTED			-2

int **CopyGraph;
int * visited;

int dfs(int node)
{
	//node has no neighbors, just count the node itself
	if(CopyGraph[node][0] == END_OF_LIST)
		return 1;

	int count = 1; //count yourself
	int i = 0;  //number of neighbors dcu has;
	while(CopyGraph[node][i] != END_OF_LIST)
	{
		int canidate = CopyGraph[node][i];
		if(canidate != DISCONNECTED && visited[canidate] == 0)
		{
			visited[canidate] = 1;
			count += dfs(canidate); //count neighbors recursively
			printf("visited: %d from %d \t count: %d\n", canidate, node, count);
		}
		i++;  //move to next neighbor
	}
	return count;
}

int main(int argc, char **argv)
{
	FILE *in = fopen("Graphs/graph2.txt", "r");
	CopyGraph = getGraph(in);

	int nodeNum = 0;
	while(CopyGraph[nodeNum][0] != -1)
		nodeNum++;

	visited = (int *)malloc(nodeNum*(sizeof(int)));
	memset(visited, 0, nodeNum*sizeof(int));
	visited[0] = 1;

	printf("nodes by dfs: %d\n", dfs(0));

	for(int i = 0; i < nodeNum; i++)
		if(visited[i] == 0)
			printf("not visited: %d\n", i);
}
