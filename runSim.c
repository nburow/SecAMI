/*
 * runSim.c
 *
 *  Created on: Dec 12, 2013
 *      Author: nburow
 */
#include <stdlib.h>
#include <stdio.h>

#define nodeNum 100
#define connections 10
#define times 10

int main(int argc, char **argv)
{
	int startGraphNum = atoi(argv[1]);
	int lastGraphNum = atoi(argv[2]);

	int compromiseTime[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	int hopTime = 1;

	double ratio[9] = {1/5, 1/4, 1/3, 1/2, 1, 2, 3, 4, 5};

	//graphs
	for(int i = startGraphNum; i < lastGraphNum; i++)
	{
		char graph[30];
		sprintf(graph, "Graphs/graph%d", i);
		strcat(graph, ".txt");

		//attack time
		for(int j = 0; j < 10; j++)
		{
			//start node
			for(int m = 1; m < nodeNum; m++)
			{
				//runs per start point
				for(int n = 0; n < 10; n++)
				{
					char cmd[20];
					sprintf(cmd, "./attackSim %d", m);
					system(cmd);
				}
			}
		}
	}
}


