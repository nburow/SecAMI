#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "attackSim.h"
#include "bfs.h"
#include "Heap.h"
#include "Event.h"

int** Graph;
int nodeNum;

int** CopyGraph;
int* bfsArray;

int** PreNodeTable;

int* NodeStatus;
Heap EventHeap;

void nodeNumOfGraph()
{
	int i;
	for (i = 0; Graph[i][0] != -1; i++);
	nodeNum = i;
}

void initial(int attackNode)
{
	//Graph Initialize
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
	nodeNumOfGraph();
//	bfsArray = bfs(attackNode, Graph);
//	bfsPrint(bfsArray);

	//PreNodeTable Initialize

	//NodeStatus Initialize
	NodeStatus = (int*)malloc(sizeof(int)*nodeNum);
	memset(NodeStatus, GOOD, sizeof(int)*nodeNum);

	//Heap Initialize
	EventHeap = (Heap)malloc(sizeof(Heap));
	EventHeap->array = (char*)malloc(sizeof(Event*) * 100);
	EventHeap->currentSize = 0;
	EventHeap->maxSize = 100;

	//Attack Initialize
	Event* event = (Event*)malloc(sizeof(Event));
	event->type = COMPROMISE;
	event->time = 0.1;
	event->subject = attackNode;
	insertNode(&EventHeap, event);
}

void disconnect(int node)
{
	int* neighbourList = CopyGraph[node];
	int i, j;

	//neighbours of node
	for (i = 1; neighbourList[i] != -1; i++)
	{
		//neighbour has already been disconneted
		if (neighbourList[i] == DISCONNECTED)	continue;
		//pick one neighbour of node and find its neighbours
		int* neighbourListOfNeighbour = CopyGraph[neighbourList[i]];				
		//disconnect this neighbour from node
		for (j = 1; neighbourListOfNeighbour[j] != -1; j++)		
		{
			if (neighbourListOfNeighbour[j] == node)
			{
				printf("disconnect %d from %d\n", neighbourList[i], node);
				neighbourListOfNeighbour[j] = DISCONNECTED;
				break;
			}
		}
		//sort of directed graph, shouldn't happen
		if (neighbourListOfNeighbour[j] == -1)
		{
			printf("Should never come here\n");
			exit(-1);
		}
		
		//disconnect node from his neighbour
		printf("disconnect %d from %d\n", node, neighbourList[i]);
		neighbourList[i] = DISCONNECTED;
	}
	printf("Now the graph is like:\n");
	myprint(CopyGraph);
	printf("--------------------------------------------\n");
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
		//pick an event from EventHeap
		Event event;
		HeapPop(EventHeap, &event);
		if (event.type == NOEVENT)
		{
			printf("No event\n");
			break;
		}
		//set current time to event time
		current = event.time;
		eventTime = 0.0;

		printf("At %f second comes event %d\n", current, event.type);

		//deal with event
		switch (event.type)	{
			case COMPROMISE:	{
				if (NodeStatus[event.subject] == COMPROMISED)
				{
					printf("WTF, %d has already been compromised\n", event.subject);
					break;
				}
				NodeStatus[event.subject] = COMPROMISED;

				// schedule hop
				printf("node %d is compromised\n", event.subject);
				printf("%f second later hop to node:\n", HOPTIME);
				
				// neighbours of the node
				int* neighbourList = CopyGraph[event.subject];
				int i;
				for (i = 1; neighbourList[i] != -1; i++)
				{
					if (NodeStatus[neighbourList[i]] == COMPROMISED)	continue;
					else if (neighbourList[i] == DISCONNECTED)			continue;
					else	printf("%d ", neighbourList[i]);
				}
				printf("\n");
				for (i = 1; neighbourList[i] != -1; i++)		//hop to next
				{
					// if neighbour is compromised but not detected or detected(disconnected)
					if (NodeStatus[neighbourList[i]] == COMPROMISED || neighbourList[i] == DISCONNECTED)	continue;
					
					Event* hopEvent = (Event*)malloc(sizeof(Event));
					hopEvent->time = current + HOPTIME;
					hopEvent->type = HOP;
					hopEvent->subject = neighbourList[i];
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
				if (NodeStatus[event.subject] != COMPROMISED)
				{
					printf("What? No wrong with node %d, dectect nothing\n", event.subject);
					exit(-1);
				}

				// set node to detected
				NodeStatus[event.subject] = DETECTED;
				// disconnect its neighbour
				printf("node: %d is detected\n", event.subject);
				printf("disconnect its neighbour:\n");
				
				int* neighbourList = CopyGraph[event.subject];
				int i;
				for (i = 1; neighbourList[i] != -1; i++)
				{
					// if has already disconnected, continue
					if (neighbourList[i] == DISCONNECTED)		continue;
					else	printf("%d ", neighbourList[i]);
				}
				printf("\n");
				disconnect(event.subject);
				break;
			}
			case HOP:	{
				// if this node has already been compromised before others spread here
				if (NodeStatus[event.subject] == COMPROMISED)		
				{
					printf("HAha, one step late, %d has been compromised\n", event.subject);
					break;
				}
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



