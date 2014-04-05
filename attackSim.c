#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
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

int whichModel;

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

	fprintf(Results, "%20d%20f\n", nodeNum-count, (count/(double)nodeNum)*100);
}

int randomChoose(int total)
{
	if (total == 0)		exit(-1);
	return (int)(uniform(&seed)*100) % total;
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

void initial(char* fileName, int attackNode, double ct, double ht, double dt, int numOfNodesToCom)
{
	fprintf(Results, "%20s%20f%20f%20f%20f%20d", fileName, ct, ht, dt, ct/dt, attackNode);
	COMTIME = ct;
	HOPTIME = ht;
	DECTIME = dt;

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
	//myprint(Log, Graph);

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
	EventHeap = (Heap)malloc(sizeof(struct myHeap));
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

	insertNode(&EventHeap, event);

	addToActiveList(event);

	//fprintf(Log, "Initialization done, start attack\n");
}

Boolean EventIsActive(Event* event)
{
	ActiveNodePtr head = AEL[event->subject]->next;
	while (head != NULL)
	{
		Event* temp = head->event;
		if (temp->type == event->type && temp->subject == event->subject && temp->object == event->object && head->active == TRUE)	return TRUE;
		head = head->next;
	}
	return FALSE;
}

void addToActiveList(Event* event)
{
	//fprintf(Log, "Adding Event %d to active list(from %d to %d)\n", event->type, event->subject, event->object);
	ActiveNodePtr newActiveEvent= (ActiveNodePtr)malloc(sizeof(ActiveNode));
	Event* temp = (Event*)malloc(sizeof(Event));
	temp->subject = event->subject;
	temp->object = event->object;
	temp->type = event->type;
	temp->time = event->time;
	newActiveEvent->event = temp;
	newActiveEvent->next = (AEL[event->subject])->next;
	newActiveEvent->active = TRUE;
	(AEL[event->subject])->next = newActiveEvent;
}

void DelDeadEvent()
{
	for (int i = 0; i < nodeNum; i++)
	{
		ActiveNodePtr pre = AEL[i];
		ActiveNodePtr current = AEL[i]->next;
		while (current != NULL)
		{
			if (current->active == FALSE)
			{
				pre->next = current->next;
				free(current->event);
				free(current);
				current = pre->next;
			}
			else
			{	
				printf("Error\n");
				exit(-1);
			}
		}
	}
}

void ShowActiveEvent()
{
	for (int i = 0; i < nodeNum; i++)
	{
		ActiveNodePtr pre = AEL[i];
		ActiveNodePtr current = AEL[i]->next;
		while (current != NULL)
		{
			if (current->active == TRUE)
			{
				Event* temp = current->event;
				printf("Active one: type %d from %d to %d at time %f\n", temp->type, temp->subject, temp->object, temp->time);
			}
			pre = current;
			current = current->next;
		}
	}
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
		else if (temp->subject == event->subject && temp->object == event->object && temp->type == event->type && current->active == TRUE)
		{
			//fprintf(Log, "Deactivating event %d from %d to %d\n", event->type, event->subject, event->object);
			current->active = FALSE;
			break;
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
		else if (current->active == TRUE)
		{
			//fprintf(Log, "stop attacking from node %d to node %d\n", temp->subject, temp->object);
			current->active = FALSE;
		}
		pre = current;
		current = current->next;
	}
}

void DCUCompromised()
{
	for (int i = 0; i < nodeNum; i++)
	{
		clearActiveList(i);
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

void freeall()
{
	//Free EventHeap
	while (!HeapIsEmpty(EventHeap))
	{
		Event* temp = (Event*)malloc(sizeof(Event));
		HeapPop(EventHeap, temp);
		free(temp);
	}
	
	free(EventHeap->array);
	free(EventHeap);
	EventHeap = NULL;

	//Free ActiveEventList
		
	for (int i = 0; i < nodeNum; i++)	DelDeadEvent();

	ShowActiveEvent();
			
	for (int i = 0; i < nodeNum; i++)
	{
		free(AEL[i]);
		AEL[i] = NULL;
	}
	free(AEL);
	AEL = NULL;

	//Free NodeStatus
	free(NodeStatus);
	NodeStatus = NULL;
	
	//Free Graph
	freeGraph(CopyGraph);
	freeGraph(Graph);	
	CopyGraph = NULL;
	Graph = NULL;
}

void runSim(char* fileName, int attackNode, double ct, double ht, double dt, int numOfNodesToCom)
{
	initial(fileName, attackNode, ct, ht, dt, numOfNodesToCom);

	double current = 0.0;
	double eventTime = 0.0;

	do
	{
		if (NodeStatus[0] == COMPROMISED || NodeStatus[0] == DETECTED)
		{
			DCUCompromised();
			break;
		}

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
		cleanActiveList(event);
		//deal with event
		switch (event->type)	{
		case COMPROMISE:	{
			
			NodeStatus[event->subject] = COMPROMISED;
			NodeStatus[event->object] = COMPROMISED;
			
			if (event->object == 0)		break;
			//fprintf(Log, "node %d is compromised\n", event->object);
	
		
			if (whichModel == 1)	{

			int i, j;
			for (i = 1; i < nodeNum; i++)
			{
				if (NodeStatus[i] == COMPROMISED)
				{
					// neighbours of the node
					int* neighbourList = CopyGraph[i];
					int stillSafe = 0;
					for (j = 1; neighbourList[j] != -1; j++)		//hop to next
					{
						if (neighbourList[j] != DISCONNECTED && NodeStatus[neighbourList[j]] == GOOD)	stillSafe++;
					}
					if (stillSafe == 0)	continue;

					else
					{
						NodeStatus[i] = HOPPING;						
						int luckyDog = randomChoose(stillSafe);
						int j;
						for (j = 1; neighbourList[j] != END_OF_LIST; j++)
						{
							if (neighbourList[j] != DISCONNECTED && NodeStatus[neighbourList[j]] == GOOD)	
							{
								if (luckyDog == 0)
								{
									//fprintf(Log, "%f second later hop to %d\n",  HOPTIME, neighbourList[j]);
									NodeStatus[neighbourList[j]] = HOPPING;
									Event* hopEvent = (Event*)malloc(sizeof(Event));
									hopEvent->time = current + HOPTIME;
									hopEvent->type = HOP;
									hopEvent->subject = i;
									hopEvent->object = neighbourList[j];
									insertNode(&EventHeap, hopEvent);
	
									addToActiveList(hopEvent);
									break;
								}
								else	luckyDog--;
							}
						}			
						if (neighbourList[j] == END_OF_LIST)
						{	
							printf("should never come here 3\n");
							exit(-1);
						}
					}
				}
			}
			eventTime = HOPTIME;
			}
			
			else if (whichModel == 2)	{

			int* neighbourList = CopyGraph[event->object];
			int stillSafe = 0;
			for (int i = 1; neighbourList[i] != END_OF_LIST; i++)
			{
				if(neighbourList[i] != DISCONNECTED && NodeStatus[neighbourList[i]] == GOOD)	stillSafe++;
			}
			if (stillSafe == 0)
			{
				//fprintf(Log, "All of the neighbours are COMPROMISED\n");
			}
			else
			{
				NodeStatus[event->object] = HOPPING;
				int temp = (numOfNodesToCom == 0) ? (randomChoose(5)) : numOfNodesToCom;
		
				int min = (stillSafe < temp) ? stillSafe : temp;
				for (int i = 0; i < min; i++)
				{
					int luckyDog = randomChoose(stillSafe);
					int j;
					for (j = 1; neighbourList[j] != END_OF_LIST; j++)
					{
						if (neighbourList[j] != DISCONNECTED && NodeStatus[neighbourList[j]] == GOOD)	
						{
							if (luckyDog == 0)
							{
								//fprintf(Log, "%f second later hop to %d\n",  HOPTIME, neighbourList[j]);
								NodeStatus[neighbourList[j]] = HOPPING;
								Event* hopEvent = (Event*)malloc(sizeof(Event));
								hopEvent->time = current + HOPTIME;
								hopEvent->type = HOP;
								hopEvent->subject = event->object;
								hopEvent->object = neighbourList[j];
								insertNode(&EventHeap, hopEvent);

								addToActiveList(hopEvent);
								break;
							}
							else	luckyDog--;
						}
					}			
					if (neighbourList[j] == END_OF_LIST)
					{	
						printf("should never come here 3\n");
						exit(-1);
					}
					stillSafe--;
				}
			}
			eventTime = HOPTIME;
			}

			// schedule detection
			double detectionTime = calDetectionTime(0, event->object);
			//fprintf(Log, "%f second should be detected\n", current+detectionTime);

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
			// set node to detected
			NodeStatus[event->object] = DETECTED;

			// disconnect its neighbour
			//fprintf(Log, "node: %d is detected\n", event->object);
			//fprintf(Log, "disconnect its neighbour:\n");

			int* neighbourList = CopyGraph[event->object];
			int i;
			for (i = 1; neighbourList[i] != -1; i++)
			{
				// if has already disconnected, continue
				if 	(neighbourList[i] == DISCONNECTED)		continue;
				else	;//fprintf(Log, "%d ", neighbourList[i]);
			}
			//fprintf(Log, "\n");
			disconnect(event->object);
			break;
		}
		case HOP:	{		
			//fprintf(Log, "reach node: %d\n", event->object);
			
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
	} while(1);
	statistic(current);
	fclose(Log);
	freeall();
}

//argv1: start graph
//argv2: end graph
//argv3: graph directory
//argv4: results file
//argv5: number of starting points to use
int main(int args, char** argv)
{
	seed = 42397952317;
	if (args < 6)
	{
		printf("6 arguments Needed\n");
		return -1;
	}
	
//	int compromiseTime[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
//	int detectTime[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

	double compromiseTime = 0.0, detectTime = 0.0, hopTime = 0.0;
	
	Results = fopen(argv[4], "w");
	fprintf(Results, "%20s%20s%20s%20s%20s%20s%20s%20s\n", "Graph File", "Compromise Time", "Hop Time", "Detect Time", "Ratio", "Start Point", "Num Compromised", "% Alive");

	printf("Specify the hop time: (second)\n");
	scanf("%lf", &hopTime);

	printf("Specify the attack time: (second)\n");
	scanf("%lf", &compromiseTime);
	
	printf("Specify the detect time: (second)\n");
	scanf("%lf", &detectTime);

	int startGraphNum = atoi(argv[1]);
	int lastGraphNum = atoi(argv[2]);
	
	whichModel = 2;
	
	/*if (whichModel != 1 && whichModel != 2)
	{
		printf("choose from model 1 or 2\n");
		exit(-1);
	}*/
	
	int numOfNodesToCom = INT_MAX;
	
	int count = 0;
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
				for(int m = 0; m < atoi(argv[5]); m++)
				{
					//runs per start point
					for(int n = 0; n < 1; n++)
					{
						printf("running sim %d\n", count++);
						runSim(graph, m, compromiseTime, hopTime, detectTime, numOfNodesToCom);
					}
				}
			}
		}
	}
	fclose(Results);

//	runSim("Graph10010/graph0.txt", 50, 1, 1, 1, 1);
	return 0;
}



