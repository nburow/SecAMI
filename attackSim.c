#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "attackSim.h"
#include "bfs.h"
#include "Heap.h"
#include "Event.h"
#include "random.h"


double COMTIME;
double HOPTIME;
double DECTIME;

FILE* Log;
FILE* Results;

int** Graph;
int nodeNum;

int** CopyGraph;

ActiveEventList AEL;
int* NodeStatus;
Heap EventHeap;

double seed;

int *visited;

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
			//fprintf(Log, "%d ", canidate);
		}
		i++;  //move to next neighbor
	}
	return count;
}

void statistic(double time)
{
	//fprintf(Log, "<-----------Collection------------->\n");
	//myprint(Log, CopyGraph);
	//fprintf(Log, "Attack simulation time: %f\n", time);
	//fprintf(Log, "Safe Nodes: \n");
	visited = (int *)malloc(nodeNum*(sizeof(int)));
	memset(visited, 0, nodeNum*sizeof(int));
	visited[0] = 1;

	int count = 0;
	if(NodeStatus[0] == DETECTED || NodeStatus[0] == COMPROMISED)
		count = 0;
	else
		count = dfs(0);

	/*fprintf(Log, "\n");
	fprintf(Log, "%d nodes out of %d nodes are safe\n", count, nodeNum);

	count = 0;
	for (int i = 0; i < nodeNum; i++)
	{
		if (NodeStatus[i] != COMPROMISED && NodeStatus[i] != DETECTED)
		{
			//fprintf(Log, "%d ", i);
			count++;
		}
	}
	fprintf(Log, "old style nodes safe: %d\n", count);*/
	//fprintf(Log, "%f%% network are compromised\n", ((double)(nodeNum-count)/(double)nodeNum)*100);

	fprintf(Results, "%20d%20f\n", nodeNum-count, (count/(double)nodeNum)*100);
}

int randomChoose(int* array)
{
	int stillSafe = 0;
	int i;
	for (i = 1; array[i] != -1; i++)
	{
		if (array[i] != DISCONNECTED && NodeStatus[array[i]] != COMPROMISED && NodeStatus[array[i]] != COMPROMISING)	stillSafe++;
	}
	if (stillSafe == 0)
	{
		printf("No\n");
		exit(-1);
	}
	int choosen = (int)(uniform(&seed)*100) % stillSafe;
	for (i = 1; array[i] != -1; i++)
	{
		if (array[i] != DISCONNECTED && NodeStatus[array[i]] != COMPROMISED && NodeStatus[array[i]] != COMPROMISING)
		{
			if (choosen == 0)	return array[i];
			else				choosen--;
		}
	}
	return WRONG; 
}

double calDetectionTime(int dcu, int compromisedNode)
{
	int distance = dijkstra(dcu, compromisedNode, Graph);
	return distance * HOPTIME + DECTIME;
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

void initial(char* fileName, int attackNode, double ct, double ht, double dt)
{
	fprintf(Results, "%20s%20f%20f%20f%20f%20d", fileName, ct, ht, dt, ct/dt, attackNode);
	COMTIME = ct;
	HOPTIME = ht;
	DECTIME = dt;

	seed = 42397952317;
	//redirect to file
	Log = fopen("log.txt", "w");
	if (!Log)
	{
		printf("File doesn't exit\n");
		exit(-1);
	}
	//Graph Initialize
	//fprintf(Log, "Initializing graph...\n");
	FILE* in = fopen(fileName, "r");
	if (!in)
	{
		printf("File doesn't exit\n");
		exit(-1);
	}
	Graph = getGraph(in);
	fclose(in);
	in = fopen(fileName, "r");
	CopyGraph = getGraph(in);
	fclose(in);

	nodeNumOfGraph();
	////myprint(Log, Graph);

	//ActiveEventList Initialize
	//fprintf(Log, "Initializing ActiveEventList...\n");
	AEL = (ActiveEventList)malloc(sizeof(ActiveNodePtr)*nodeNum);
	initialActiveEventList();

	//NodeStatus Initialize
	//fprintf(Log, "Initializing NodeStatus...\n");
	NodeStatus = (int*)malloc(sizeof(int)*nodeNum);
	memset(NodeStatus, GOOD, sizeof(int)*nodeNum);

	//Heap Initialize
	//fprintf(Log, "Initializing Heap...\n");
	EventHeap = (Heap)malloc(sizeof(Heap));
	EventHeap->array = (Event**)malloc(sizeof(Event*) * 100);
	EventHeap->currentSize = 0;
	EventHeap->maxSize = 100;

	//Attack Initialize
	//fprintf(Log, "Initializing Attack...\n");
	Event* event = (Event*)malloc(sizeof(Event));
	event->type = COMPROMISE;
	event->time = 0.1;
	event->subject = attackNode;
	event->object = attackNode;

	/*	Event* event1 = (Event*)malloc(sizeof(Event));
	event1->type = COMPROMISE;
	event1->time = 0.1;
	event1->subject = 3;
	event1->object = 3;
	 */
	insertNode(&EventHeap, event);
	//	insertNode(&EventHeap, event1);

	addToActiveList(event);
	//	addToActiveList(event1);

	//fprintf(Log, "Initialization done, start attack\n");
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
	//fprintf(Log, "Adding attack to active list(from %d to %d)\n", event->subject, event->object);
	ActiveNodePtr newActiveEvent= (ActiveNodePtr)malloc(sizeof(ActiveNode));
	newActiveEvent->event = event;
	newActiveEvent->next = (AEL[event->subject])->next;
	newActiveEvent->active = TRUE;
	(AEL[event->subject])->next = newActiveEvent;
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
			//fprintf(Log, "should not come here\n");
			exit(-1);
		}
		if (current->active == FALSE)
		{
			//fprintf(Log, "cleaning the activelist...\n");
			pre->next = current->next;
			free(current);
			current = pre->next;
			continue;
		}
		else if (temp->subject == event->subject && temp->object == event->object && temp->type == event->type && current->active == TRUE)
		{
			//fprintf(Log, "Deactivating event %d from %d to %d\n", event->type, event->subject, event->object);
			current->active = FALSE;
		}
		pre = current;
		current = current->next;
	}
}

void clearActiveList(int node)
{
	//fprintf(Log, "Deactivating event conerning with node %d...\n", node);
	ActiveNodePtr pre = AEL[node];
	ActiveNodePtr current = AEL[node]->next;
	while (current != NULL)
	{
		Event* temp = current->event;
		if (temp == NULL)
		{
			//fprintf(Log, "should not come here 2\n");
			exit(-1);
		}
		if (current->active == FALSE)
		{
			//fprintf(Log, "cleaning the activelist...\n");
			pre->next = current->next;
			free(current);
			current = pre->next;
		}
		else
		{
			//fprintf(Log, "stop attacking from node %d to node %d\n", temp->subject, temp->object);
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
				//fprintf(Log, "disconnect %d from %d\n", neighbourList[i], node);
				neighbourListOfNeighbour[j] = DISCONNECTED;
				break;
			}
		}
		//sort of directed graph, shouldn't happen
		if (neighbourListOfNeighbour[j] == -1)
		{
			//fprintf(Log, "Should never come here\n");
			exit(-1);
		}

		//disconnect node from his neighbour
		//fprintf(Log, "disconnect %d from %d\n", node, neighbourList[i]);
		neighbourList[i] = DISCONNECTED;
	}

	clearActiveList(node);

	//fprintf(Log, "Now the graph is like:\n");
	//myprint(Log, CopyGraph);
	//fprintf(Log, "--------------------------------------------\n");
}

void runSim(char* fileName, int attackNode, double ct, double ht, double dt)
{
	initial(fileName, attackNode, ct, ht, dt);

	double current = 0.0;
	double eventTime = 0.0;

	do
	{
		if (NodeStatus[0] == COMPROMISED)		break;
		//pick an event from EventHeap
		Event* event = HeapGetMin(EventHeap);
		if (event == NULL)		break;
		if (!EventIsActive(event))
		{
			//fprintf(Log, "Event has been cancelled, delete from the heap\n");
			HeapDelMin(EventHeap);
			continue;
		}

		//set current time to event time
		current = event->time;
		eventTime = 0.0;

		//fprintf(Log, "At %f second comes event %d\n", current, event->type);

		//deal with event
		switch (event->type)	{
		case COMPROMISE:	{
			if (NodeStatus[event->object] == COMPROMISED)
			{
				//fprintf(Log, "WTF, %d has already been compromised\n", event->object);
				break;
			}
			NodeStatus[event->subject] = COMPROMISED;
			NodeStatus[event->object] = COMPROMISED;

			//fprintf(Log, "node %d is compromised\n", event->object);
			cleanActiveList(event);

			// search target to compromise
			int i, j;
			int whichNeighbour = 0;
			for (i = 0; i < nodeNum; i++)
			{
				if (NodeStatus[i] == COMPROMISED)
				{
					// neighbours of the node
					int* neighbourList = CopyGraph[i];
					for (j = 1; neighbourList[j] != -1; j++)		//hop to next
					{
						// if neighbour is compromised but not detected or detected(disconnected) or is under attack or is attacking
						if (NodeStatus[neighbourList[j]] == COMPROMISED || neighbourList[j] == DISCONNECTED || NodeStatus[neighbourList[j]] == COMPROMISING || NodeStatus[neighbourList[j]] == HOPPING)
						{
							continue;
						}
						// there's a node to hop to
						else
						{
							whichNeighbour = randomChoose(neighbourList);

							if (whichNeighbour == NOONE)
							{
								printf("ALL nodes are occupied\n");
								continue;
							}
							if (whichNeighbour == WRONG)
							{
								printf("crash!!!!\n");
								exit(-1);
							}

							//fprintf(Log, "%f second later hop to %d\n", HOPTIME,whichNeighbour);
							break;
							// or you can count the reachable neighbours for lately random choose
						}
					}
					if (neighbourList[j] == -1)
					{
						//fprintf(Log, "All of the neighbours are COMPROMISED\n");
						continue;
					}

					NodeStatus[i] = HOPPING;
					NodeStatus[whichNeighbour] = HOPPING;
					// then hop to it
					Event* hopEvent = (Event*)malloc(sizeof(Event));
					hopEvent->time = current + HOPTIME;
					hopEvent->type = HOP;
					hopEvent->subject = i;
					hopEvent->object = whichNeighbour;
					//						hopEvent->object = neighbourList[j];
					insertNode(&EventHeap, hopEvent);

					addToActiveList(hopEvent);

				}
			}
			eventTime = HOPTIME;				//time needed for event to happen

			// schedule detection
			double detectionTime = calDetectionTime(0, event->object);
			//fprintf(Log, "%f second should be detected\n", current+detectionTime);

			NodeStatus[0] = DETECTING;

			Event* detectEvent = (Event*)malloc(sizeof(Event));
			detectEvent->time = current + detectionTime;
			detectEvent->type = DETECTION;
			// subject should be DCU, which is supposed to be at node 0
			detectEvent->subject = 0;
			detectEvent->object = event->object;
			insertNode(&EventHeap, detectEvent);

			addToActiveList(detectEvent);
			if (eventTime < detectionTime)	eventTime = detectionTime;

			break;
		}
		case DETECTION:		{
			if (NodeStatus[event->object] != COMPROMISED && NodeStatus[event->object] != HOPPING && NodeStatus[event->object] != COMPROMISING && NodeStatus[event->object] != DETECTED && event->object != 0)
			{
				printf("status: %d\n", NodeStatus[event->object]);
				//fprintf(Log, "What? No wrong with node %d, dectect nothing\n", event->object);
				exit(-1);
			}

			// set node to detected
			NodeStatus[event->object] = DETECTED;

			cleanActiveList(event);
			// disconnect its neighbour
			//fprintf(Log, "node: %d is detected\n", event->object);
			//fprintf(Log, "disconnect its neighbour:\n");

			int* neighbourList = CopyGraph[event->object];
			int i;
			for (i = 1; neighbourList[i] != -1; i++)
			{
				// if has already disconnected, continue
				if (neighbourList[i] == DISCONNECTED)		continue;
				else	;//fprintf(Log, "%d ", neighbourList[i]);
			}
			//fprintf(Log, "\n");
			disconnect(event->object);
			break;
		}
		case HOP:	{
			// if this node has already been compromised before others spread here
			if (NodeStatus[event->object] == COMPROMISED)
			{
				//fprintf(Log, "HAha, one step late, %d has been compromised\n", event->object);
				break;
			}
			//fprintf(Log, "reach node: %d\n", event->object);
			cleanActiveList(event);

			NodeStatus[event->subject] = COMPROMISING;
			NodeStatus[event->object] = COMPROMISING;

			//fprintf(Log, "%f second later compromise node %d\n", COMTIME, event->object);
			Event* compromiseEvent = (Event*)malloc(sizeof(Event));
			compromiseEvent->time = current + COMTIME;
			compromiseEvent->type = COMPROMISE;
			compromiseEvent->subject = event->subject;
			compromiseEvent->object = event->object;
			insertNode(&EventHeap, compromiseEvent);

			addToActiveList(compromiseEvent);
			eventTime = COMTIME;
			break;
		}
		default:	break;
		}
		HeapDelMin(EventHeap);
		//		free(event);
	} while(1);

	statistic(current);
	fclose(Log);
}
//args1: first graph #
//args2: last graph #
//args3: graph directory
//args4: result file name
int main(int args, char** argv)
{
	/*	if (args < 6)
	{
		printf("5 arguments Needed\n");
		return -1;
	}
	 */
	int startGraphNum = atoi(argv[1]);
	int lastGraphNum = atoi(argv[2]);

	Results = fopen(argv[4], "w");
	fprintf(Results, "%20s%20s%20s%20s%20s%20s%20s%20s\n", "Graph File", "Compromise Time", "Hop Time", "Detect Time", "Ratio",
			"Start Point", "Num Compromised", "% Alive");

	int compromiseTime[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	int detectTime[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	int hopTime = 1;

	int count = 0;


	for(int i = 0; i < 10; i++)
	{
		for(int j = 0; j < 10; j++)
		{
			if(detectTime[j] != 0)
				printf("%f\n", compromiseTime[i]/(double)detectTime[j]);
		}

	}
	//graphs
	for(int i = startGraphNum; i <= lastGraphNum; i++)
	{
		char graph[30];
		sprintf(graph, "%s/graph%d", argv[3], i);
		strcat(graph, ".txt");
		printf("%s\n", graph);
		//compromise times
		for(int j = 0; j < 10; j++)
		{
			//detect times
			for(int k = 0; k < 10; k++)
			{
				//start node
				for(int m = 0; m < 100; m++)
				{
					//runs per start point
					for(int n = 0; n < 2; n++)
					{
						printf("running sim %d\n", count++);
						runSim(graph, m, compromiseTime[j], hopTime, detectTime[k]);
					}
				}
			}
		}
	}
	fclose(Results);
	return 0;
}



