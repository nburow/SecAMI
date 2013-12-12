#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "attackSim.h"
#include "bfs.h"
#include "Heap.h"
#include "Event.h"


FILE* Log;

int** Graph;
int nodeNum;

int** CopyGraph;
int* bfsArray;

ActiveEventList AEL;
int* NodeStatus;
Heap EventHeap;


double calDetectionTime(int dcu, int compromisedNode)
{
	int distance = dijkstra(dcu, compromisedNode, Graph);
	return distance * HOPTIME;
}

void nodeNumOfGraph()
{
	int i;
	for (i = 0; Graph[i][0] != -1; i++);
	nodeNum = i;
}

void initialActiveEventList()
{
	int i;
	for (i = 0 ; i < nodeNum; i++)
	{
		ActiveNodePtr header = (ActiveNodePtr)malloc(sizeof(ActiveNode));
		header->event = NULL;
		header->next = NULL;
		header->active = FALSE;
		AEL[i] = header;
	}
}

void initial(int attackNode)
{
	//redirect to file
	Log = fopen("log.txt", "w");
	if (!Log)
	{
		fprintf(Log, "File doesn't exit\n");
		exit(-1);
	}
	//Graph Initialize
	fprintf(Log, "Initializing graph...\n");
	FILE* in = fopen("graph.txt", "r");
	if (!in)
	{
		fprintf(Log, "File doesn't exit\n");
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
	
	//ActiveEventList Initialize
	fprintf(Log, "Initializing ActiveEventList...\n");
	AEL = (ActiveEventList)malloc(sizeof(ActiveNodePtr)*nodeNum);
	initialActiveEventList();

	//NodeStatus Initialize
	fprintf(Log, "Initializing NodeStatus...\n");
	NodeStatus = (int*)malloc(sizeof(int)*nodeNum);
	memset(NodeStatus, GOOD, sizeof(int)*nodeNum);

	//Heap Initialize
	fprintf(Log, "Initializing Heap...\n");
	EventHeap = (Heap)malloc(sizeof(Heap));
	EventHeap->array = (Event**)malloc(sizeof(Event*) * 100);
	EventHeap->currentSize = 0;
	EventHeap->maxSize = 100;

	//Attack Initialize
	fprintf(Log, "Initializing Attack...\n");
	Event* event = (Event*)malloc(sizeof(Event));
	event->type = COMPROMISE;
	event->time = 0.1;
	event->subject = attackNode;
	event->object = attackNode;
//	event->active = TRUE;
	insertNode(&EventHeap, event);

	addToActiveList(event);
	fprintf(Log, "Initialization done, start attack\n");
}

Boolean EventIsActive(Event* event)
{
	ActiveNodePtr head = AEL[event->subject]->next;
	while (head != NULL)
	{
		Event* temp = head->event;
		if (temp->subject == event->subject && temp->object == event->object)
		{
			if (head->active == TRUE)	return TRUE;
		}
		head = head->next;
	}
	return FALSE;
}

void addToActiveList(Event* event)
{
//	fprintf(Log, "Adding attack to active list(from %d to %d)\n", event->subject, event->object);
	ActiveNodePtr newActiveEvent= (ActiveNodePtr)malloc(sizeof(ActiveNode));
	newActiveEvent->event = event;
	newActiveEvent->next = (AEL[event->subject])->next;
	newActiveEvent->active = TRUE;
	(AEL[event->subject])->next = newActiveEvent;
	
/*	
	fprintf(Log, "..........................\n");
	ActiveNodePtr ptr = AEL[event->subject]->next;
	while (ptr != NULL)
	{
		Event* temp = ptr->event;
		if (temp == NULL)
		{
			fprintf(Log, "~~~~~~~~~\n");
		}
		else if (ptr->active == TRUE)
		{
			fprintf(Log, "test: %d to %d\n", (ptr->event)->subject, (ptr->event)->object);
		}
		ptr = ptr->next;
	}
	fprintf(Log, "..........................\n");
	*/
}

void cleanActiveList(Event* event)
{
	ActiveNodePtr pre = AEL[event->subject];
	ActiveNodePtr current = AEL[event->subject]->next;
	while (current != NULL)
	{
		Event* temp = current->event;
		if (temp == NULL)
		{
			fprintf(Log, "should not come here\n");
			exit(-1);
		}
		if (current->active == FALSE)
		{
//			fprintf(Log, "cleaning the activelist...\n");
			pre->next = current->next;
			free(current);
			current = pre->next;
			continue;
		}
		else if (temp->subject == event->subject && temp->object == event->object && temp->type == event->type && current->active == TRUE)
		{
//			fprintf(Log, "Deactivating event %d from %d to %d\n", event->type, event->subject, event->object);
			current->active = FALSE;
		}
		pre = current;
		current = current->next;
	}
}

void clearActiveList(int node)
{
//	fprintf(Log, "Deactivating event conerning with node %d...\n", node);
	ActiveNodePtr pre = AEL[node];
	ActiveNodePtr current = AEL[node]->next;
	while (current != NULL)
	{
		Event* temp = current->event;
		if (temp == NULL)
		{
			fprintf(Log, "should not come here 2\n");
			exit(-1);
		}
		if (current->active == FALSE)
		{
//			fprintf(Log, "cleaning the activelist...\n");
			pre->next = current->next;
			free(current);
			current = pre->next;
		}
		else
		{
			fprintf(Log, "stop attacking from node %d to node %d\n", temp->subject, temp->object);
			current->active = FALSE;
		
			pre = current;
			current = current->next;
		}
	}
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
				fprintf(Log, "disconnect %d from %d\n", neighbourList[i], node);
				neighbourListOfNeighbour[j] = DISCONNECTED;
				break;
			}
		}
		//sort of directed graph, shouldn't happen
		if (neighbourListOfNeighbour[j] == -1)
		{
			fprintf(Log, "Should never come here\n");
			exit(-1);
		}
		
		//disconnect node from his neighbour
		fprintf(Log, "disconnect %d from %d\n", node, neighbourList[i]);
		neighbourList[i] = DISCONNECTED;
	}

	clearActiveList(node);
	
	fprintf(Log, "Now the graph is like:\n");
	myprint(Log, CopyGraph);
	fprintf(Log, "--------------------------------------------\n");
}

int main(int args, char** argv)
{
	if (args < 2)
	{
		fprintf(Log, "specify where to start attack\n");
		return -1;
	}
	initial(atoi(argv[1]));

	double current = 0.0;
	double eventTime = 0.0;
/*	
	fprintf(Log, "cSize: %d\n", EventHeap->currentSize);
	int i;
	for (i = 0; i < EventHeap->currentSize; i++)
	{
		Event e;
		HeapPop(EventHeap, &e);
		fprintf(Log, "time: %f\ntype: %d\nsubject: %d\n", e.time, e.type, e.subject);
	}
*/
	do
	{
		//pick an event from EventHeap
		Event* event = HeapGetMin(EventHeap);
		if (event == NULL)		break;
		if (!EventIsActive(event))
		{
//			fprintf(Log, "Event has been cancelled, delete from the heap\n");
			HeapDelMin(EventHeap);
			continue;
		}

/*		if (event->type == NOEVENT)
		{
			fprintf(Log, "No event!\n");
			break;
		}
		if (event->active == FALSE)
		{
		
			HeapDelMin(EventHeap);
			continue;
		}
*/		//set current time to event time
		current = event->time;
		eventTime = 0.0;

		fprintf(Log, "At %f second comes event %d\n", current, event->type);

		//deal with event
		switch (event->type)	{
			case COMPROMISE:	{
				if (NodeStatus[event->object] == COMPROMISED)
				{
					fprintf(Log, "WTF, %d has already been compromised\n", event->object);
					break;
				}
				NodeStatus[event->object] = COMPROMISED;

				fprintf(Log, "node %d is compromised\n", event->object);
				cleanActiveList(event);

			//	fprintf(Log, "%f second later hop to node:\n", HOPTIME);
				
				// search target to compromise
				int i, j;
				for (i = 0; i < nodeNum; i++)
				{
					if (NodeStatus[i] == COMPROMISED)
					{
						// neighbours of the node
						int* neighbourList = CopyGraph[i];
						for (j = 1; neighbourList[j] != -1; j++)		//hop to next
						{
							// if neighbour is compromised but not detected or detected(disconnected)
							if (NodeStatus[neighbourList[j]] == COMPROMISED || neighbourList[j] == DISCONNECTED)	continue;
							// there's a node to hop to
							else
							{
								fprintf(Log, "%f second later hop to %d\n", HOPTIME, neighbourList[j]);
								break;
								// or you can count the reachable neighbours for lately random choose
							}
						}
						if (neighbourList[j] == -1)
						{
							fprintf(Log, "All of the neighbours are COMPROMISED\n");
							continue;
						}
						// then hop to it
						Event* hopEvent = (Event*)malloc(sizeof(Event));
						hopEvent->time = current + HOPTIME;
						hopEvent->type = HOP;
						hopEvent->subject = i;
						//hopEvent->object = randomChoose(neighbourList);
						hopEvent->object = neighbourList[j];
					//	hopEvent->active = TRUE;
						insertNode(&EventHeap, hopEvent);

						addToActiveList(hopEvent);

					}
				}
				eventTime = HOPTIME;				//time needed for event to happen
				
				// schedule detection
				double detectionTime = calDetectionTime(0, event->object);
				fprintf(Log, "%f second should be detected\n", current+detectionTime);

				Event* detectEvent = (Event*)malloc(sizeof(Event));
				detectEvent->time = current + detectionTime;
				detectEvent->type = DETECTION;
				// subject should be DCU, which is supposed to be at node 0
				detectEvent->subject = 0;
				detectEvent->object = event->object;
			//	detectEvent->active = TRUE;
				insertNode(&EventHeap, detectEvent);

				addToActiveList(detectEvent);
				if (eventTime < detectionTime)	eventTime = detectionTime;

				break;
			}
			case DETECTION:		{
				if (NodeStatus[event->object] != COMPROMISED)
				{
					fprintf(Log, "What? No wrong with node %d, dectect nothing\n", event->object);
					exit(-1);
				}

				// set node to detected
				NodeStatus[event->object] = DETECTED;
				
				cleanActiveList(event);
				// disconnect its neighbour
				fprintf(Log, "node: %d is detected\n", event->object);
				fprintf(Log, "disconnect its neighbour:\n");
				
				int* neighbourList = CopyGraph[event->object];
				int i;
				for (i = 1; neighbourList[i] != -1; i++)
				{
					// if has already disconnected, continue
					if (neighbourList[i] == DISCONNECTED)		continue;
					else	fprintf(Log, "%d ", neighbourList[i]);
				}
				fprintf(Log, "\n");
				disconnect(event->object);
				break;
			}
			case HOP:	{
				// if this node has already been compromised before others spread here
				if (NodeStatus[event->object] == COMPROMISED)		
				{
					fprintf(Log, "HAha, one step late, %d has been compromised\n", event->object);
					break;
				}
				fprintf(Log, "reach node: %d\n", event->object);
				cleanActiveList(event);

				fprintf(Log, "%f second later compromise node %d\n", COMTIME, event->object);
				Event* compromiseEvent = (Event*)malloc(sizeof(Event));
				compromiseEvent->time = current + COMTIME;
				compromiseEvent->type = COMPROMISE;
				compromiseEvent->subject = event->subject;
				compromiseEvent->object = event->object;
			//	compromiseEvent->active = TRUE;
				insertNode(&EventHeap, compromiseEvent);
				
				addToActiveList(compromiseEvent);
				eventTime = COMTIME;
				break;
			}
			default:	break;
		}
		HeapDelMin(EventHeap);
//		free(event);
	} while(current + eventTime < SIMTIME);
	fclose(Log);
	return 0;
}



