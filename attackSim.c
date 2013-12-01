#include <stdlib.h>
#include <stdio.h>
#include "attackSim.h"
#include "bfs.h"
#include "Heap.h"
#include "Event.h"

int** Graph;
int** CopyGraph;
int* bfsArray;
Heap EventHeap;

void initial(int attackNode)
{
	FILE* in = fopen("graph.txt", "r");
	if (!in)
	{
		printf("File doesn't exit\n");
		exit(-1);
	}
	Graph = getGraph(in);
	fclose(in);
	in = fopen("graph.txt", "r");
	CopyGraph = getGraph(in);
	fclose(in);
//	bfsArray = bfs(attackNode, Graph);
//	bfsPrint(bfsArray);

	EventHeap = (Heap)malloc(sizeof(Heap));
	EventHeap->array = (char*)malloc(sizeof(Event*) * 100);
	EventHeap->currentSize = 0;
	EventHeap->maxSize = 100;

	Event* event = (Event*)malloc(sizeof(Event));
	event->type = COMPROMISE;
	event->time = 0.1;
	event->subject = attackNode;
	insertNode(&EventHeap, event);
}

void disconnect(int node, int* neighbour)
{
	int* nodeList = CopyGraph[node];
	int i, j;

	for (i = 1; neighbour[i] != -1; i++)
	{
		int* neighbourList = CopyGraph[neighbour[i]];
		for (j = 1; neighbourList[j] != -1; j++)
		{
			if (neighbourList[j] == node)
			{
				printf("disconnect %d from %d\n", neighbour[i], node);
				neighbourList[j] = -2;
				break;
			}
		}
		if (neighbourList[j] == -1)					//no nodes connected to it
		{
			
		}
	}
	for (i = 1; nodeList[i] != -1; i++)		
	{
		printf("disconnect %d from %d\n", node, nodeList[i]);
		nodeList[i] = -1;
	}
	printf("Now the graph is like:\n");
	myprint(CopyGraph);
	printf("------------------\n");
}

int main(int args, char** argv)
{
	if (args < 2)
	{
		printf("specify where to start attack\n");
		return -1;
	}
	initial(atoi(argv[1]));

	double current = 0.0;
	double eventTime = 0.0;
/*	
	printf("cSize: %d\n", EventHeap->currentSize);
	int i;
	for (i = 0; i < EventHeap->currentSize; i++)
	{
		Event e;
		HeapPop(EventHeap, &e);
		printf("time: %f\ntype: %d\nsubject: %d\n", e.time, e.type, e.subject);
	}
*/
	do
	{
		Event event;
		HeapPop(EventHeap, &event);
		current = event.time;
		eventTime = 0.0;
		printf("here comes %d\n", event.type);
		switch (event.type)	{
			case COMPROMISE:	{
				// schedule hop
				int* childList = CopyGraph[event.subject];
				int i;

				printf("node %d is compromised\n", event.subject);
				printf("At %f second will hop to node:\n", current+HOPTIME);
				for (i = 1; childList[i] != -1; i++)
					printf("%d ", childList[i]);
				printf("\n");
				for (i = 1; childList[i] != -1; i++)		//hop to next
				{
					Event* hopEvent = (Event*)malloc(sizeof(Event));
					hopEvent->time = current + HOPTIME;
					hopEvent->type = HOP;
					hopEvent->subject = childList[i];
					insertNode(&EventHeap, hopEvent);

					eventTime = HOPTIME;				//time needed for event to happen
				}
				
				// schedule detection
				Event* detectEvent = (Event*)malloc(sizeof(Event));
				detectEvent->time = current + DETTIME;
				detectEvent->type = DETECTION;
				detectEvent->subject = event.subject;
				insertNode(&EventHeap, detectEvent);

				if (eventTime < DETTIME)	eventTime = DETTIME;

				break;
			}
			case DETECTION:		{
				// disconnect children
				int* childList = CopyGraph[event.subject];
				int i;

				printf("node: %d is detected\n", event.subject);
				printf("its children:\n");
				for (i = 1; childList[i] != -1; i++)
				{
					if (childList[i] == -2)		continue;
					else						printf("%d ", childList[i]);
				}
				printf("\n");
				disconnect(event.subject, childList);		// disconnect curNode's children
				break;
			}
			case HOP:	{
				printf("spread to node: %d\n", event.subject);
				Event* compromiseEvent = (Event*)malloc(sizeof(Event));
				compromiseEvent->time = current + COMTIME;
				compromiseEvent->type = COMPROMISE;
				compromiseEvent->subject = event.subject;
				insertNode(&EventHeap, compromiseEvent);

				eventTime = COMTIME;
				break;
			}
			default:	break;
		}
	} while(current + eventTime < SIMTIME);

	return 0;
}



