#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <limits.h>
#include "attackSim.h"
#include "bfs.h"
#include "Heap.h"
#include "Event.h"
#include "random.h"

#define LOG		1
#if LOG == 1 
#define LOGINITIAL()								\
do {												\
	Log = fopen("log.txt", "w");					\
	if (!Log)										\
	{												\
		printf("File doesn't exit\n");				\
		exit(-1);									\
	}												\
} while(0)
					
#define LOGPRINT(format)			 				\
do {												\
	logRecorder format;								\
} while(0)											\


#define LOGCLOSE()									\
do {												\
	fclose(Log);									\
} while(0)
#else
#define LOGINITIAL()
#define LOGPRINT(format)
#define LOGCLOSE()
#endif

double COMTIME;
double HOPTIME;
double DECTIME;
double DISTIME;

FILE* Log;
FILE* Results;

int** Graph;
int nodeNum;
int keyRound;


int** CopyGraph;

ActiveEventList AEL;
int* NodeStatus;
int* KeyChain;
Heap EventHeap;

double seed;

int *visited;

int whichModel;

void logRecorder(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	vfprintf(Log, format, args);
	va_end(args);
}

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
			LOGPRINT(("%d ", canidate));
		}
		i++;  //move to next neighbor
	}
	return count;
}

void statistic(double time)
{
	LOGPRINT(("<-----------Collection------------->\n"));
	//myprint(Log, CopyGraph);
	LOGPRINT(("Attack simulation time: %f\n", time));
	LOGPRINT(("Safe Nodes: \n"));
	visited = (int*)malloc(nodeNum*(sizeof(int)));
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

	DISTIME = 0.5;

	//redirect to file
/*	Log = fopen("log.txt", "w");
	if (!Log)
	{
		printf("File doesn't exit\n");
		exit(-1);
	}
*/

	//Log file Initialize
	LOGINITIAL();

	//Graph Initialize
	LOGPRINT(("Initializing graph...\n"));
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

	//key initialize
	keyRound = 0;

	//ActiveEventList Initialize
	LOGPRINT(("Initializing ActiveEventList...\n"));
	AEL = (ActiveEventList)malloc(sizeof(ActiveNodePtr)*nodeNum);
	initialActiveEventList();

	//NodeStatus Initialize
	LOGPRINT(("Initializing NodeStatus...\n"));
	NodeStatus = (int*)malloc(sizeof(int)*nodeNum);
	memset(NodeStatus, GOOD, sizeof(int)*nodeNum);

	//KeyChain Intialize
	LOGPRINT(("Initializing KeyChain...\n"));
	KeyChain = (int*)malloc(sizeof(int)*nodeNum);
	memset(KeyChain, KEYZERO, sizeof(int)*nodeNum);

	//Heap Initialize
	LOGPRINT(("Initializing Heap...\n"));
	EventHeap = (Heap)malloc(sizeof(struct myHeap));
	EventHeap->array = (Event**)malloc(sizeof(Event*) * 100);
	EventHeap->currentSize = 0;
	EventHeap->maxSize = 100;

	//Attack Initialize
	LOGPRINT(("Initializing Attack...\n"));
	Event* event = (Event*)malloc(sizeof(Event));
	event->type = COMPROMISE;
	event->time = 0.1;
	event->subject = attackNode;
	event->object = attackNode;
	event->keyRound = KEYZERO;

	insertNode(&EventHeap, event);

	addToActiveList(event);

	LOGPRINT(("Initialization done, start attack\n"));
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
	LOGPRINT(("Adding Event %d to active list(from %d to %d)\n", event->type, event->subject, event->object));
	ActiveNodePtr newActiveEvent= (ActiveNodePtr)malloc(sizeof(ActiveNode));
	Event* temp = (Event*)malloc(sizeof(Event));
	temp->subject = event->subject;
	temp->object = event->object;
	temp->type = event->type;
	temp->time = event->time;
	temp->keyRound = event->keyRound;
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
			printf( "should not come here\n");
			exit(-1);
		}
		else if (temp->subject == event->subject && temp->object == event->object && temp->type == event->type && current->active == TRUE)
		{
			LOGPRINT(("Deactivating event %d from %d to %d\n", event->type, event->subject, event->object));
			current->active = FALSE;
			break;
		}
		pre = current;
		current = current->next;
		
	}
}

void clearActiveList(int node)
{
	LOGPRINT(("Deactivating event conerning with node %d...\n", node));
	ActiveNodePtr pre = AEL[node];
	ActiveNodePtr current = AEL[node]->next;
	while (current != NULL)
	{
		Event* temp = current->event;
		if (temp == NULL)
		{
			printf( "should not come here 2\n");
			exit(-1);
		}
		else if (current->active == TRUE)
		{
			LOGPRINT(("stop attacking from node %d to node %d\n", temp->subject, temp->object));
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
				LOGPRINT(("disconnect %d from %d\n", neighbourList[i], node));
				neighbourListOfNeighbour[j] = DISCONNECTED;
				break;
			}
		}
		//sort of directed graph, shouldn't happen
		if (neighbourListOfNeighbour[j] == -1)
		{
			printf( "Should never come here\n");
			exit(-1);
		}

		//disconnect node from his neighbour
		LOGPRINT(("disconnect %d from %d\n", node, neighbourList[i]));
		neighbourList[i] = DISCONNECTED;
	}

	clearActiveList(node);
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

	//Free KeyChain
	free(KeyChain);
	KeyChain = NULL;
	
	//Free visted
	free(visited);
	visited = NULL;

	//Free Graph
	freeGraph(CopyGraph);
	freeGraph(Graph);	
	CopyGraph = NULL;
	Graph = NULL;
}

void runSim1(char* fileName, int attackNode, double ct, double ht, double dt, int numOfNodesToCom)
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
			LOGPRINT(("Event has been cancelled, delete from the heap\n"));
			HeapDelMin(EventHeap);
			continue;
		}

		//set current time to event time
		current = event->time;
		eventTime = 0.0;

		LOGPRINT(("At %f second comes event %d\n", current, event->type));
		cleanActiveList(event);
		//deal with event
		switch (event->type)	{
		case COMPROMISE:	{
			
			NodeStatus[event->subject] = COMPROMISED;
			NodeStatus[event->object] = COMPROMISED;
			
			if (event->object == 0)		break;
			LOGPRINT(("node %d is compromised\n", event->object));

			int* neighbourList = CopyGraph[event->object];
			int stillSafe = 0;
			for (int i = 1; neighbourList[i] != END_OF_LIST; i++)
			{
				if(neighbourList[i] != DISCONNECTED && NodeStatus[neighbourList[i]] == GOOD)	stillSafe++;
			}
			if (stillSafe == 0)
			{
				LOGPRINT(("All of the neighbours are COMPROMISED\n"));
			}
			else
			{
				NodeStatus[event->object] = HOPPING;
				int temp = (numOfNodesToCom == 0) ? (randomChoose(5)) : numOfNodesToCom;
				int min = (stillSafe < temp) ? stillSafe : temp;
	
				for (int i = 0; i < min; i++)
				{
					int j;
					int luckyDog = randomChoose(stillSafe);
					for (j = 1; neighbourList[j] != END_OF_LIST; j++)
					{
						if (neighbourList[j] != DISCONNECTED && NodeStatus[neighbourList[j]] == GOOD)	
						{
							if (luckyDog == 0)
							{
								LOGPRINT(("%f second later hop to %d\n",  HOPTIME, neighbourList[j]));
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

			// schedule detection
			double detectionTime = calDetectionTime(0, event->object);
			LOGPRINT(("%f second should be detected\n", current+detectionTime));

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
			LOGPRINT(("node: %d is detected\n", event->object));
			LOGPRINT(("disconnect its neighbour:\n"));

			disconnect(event->object);
			break;
		}
		case HOP:	{		
			LOGPRINT(("reach node: %d\n", event->object));
			
			NodeStatus[event->subject] = COMPROMISING;
			NodeStatus[event->object] = COMPROMISING;

			LOGPRINT(("%f second later compromise node %d\n", COMTIME, event->object));
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
	LOGCLOSE();
	freeall();
}

void runSim2(char* fileName, int attackNode, double ct, double ht, double dt, int numOfNodesToCom)
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
			LOGPRINT(("Event has been cancelled, delete from the heap\n"));
			HeapDelMin(EventHeap);
			continue;
		}

		//set current time to event time
		current = event->time;
		eventTime = 0.0;

		LOGPRINT(("At %f second comes event %d\n", current, event->type));
		cleanActiveList(event);
		//deal with event
		switch (event->type)	{
		case COMPROMISE:	{
			// if is already compromised, ignore 
			if (NodeStatus[event->object] == COMPROMISED || NodeStatus[event->object] == COMPROMISING || NodeStatus[event->object] == HOPPING || NodeStatus[event->object] == DETECTED)	break;
			// else if protected, authenticate
			else if (NodeStatus[event->object] == PROTECTED)
			{
				if (event->keyRound != KeyChain[event->object])
				{
					LOGPRINT(("authenticate failed\n"));
					break;
				}
			}
			
			//authentication succeed or has not been protected
			//NodeStatus[event->subject] = COMPROMISED;
			NodeStatus[event->object] = COMPROMISED;
			
			if (event->object == 0)		break;
			LOGPRINT(("node %d is compromised\n", event->object));
	
		
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
				LOGPRINT(("All of the neighbours are COMPROMISED\n"));
			}
			else
			{
				NodeStatus[event->object] = HOPPING;
				int temp = (numOfNodesToCom == 0) ? (randomChoose(5)) : numOfNodesToCom;
		
				int min = (stillSafe < temp) ? stillSafe : temp;
				int luckyDog = randomChoose(stillSafe);
				for (int i = 0; i < min; i++)
				{
					int j;
					int temp = (luckyDog+i) % min;
					for (j = 1; neighbourList[j] != END_OF_LIST; j++)
					{
						if (neighbourList[j] != DISCONNECTED && NodeStatus[neighbourList[j]] == GOOD)	
						{
							if (temp == 0)
							{
								LOGPRINT(("%f second later hop to %d\n",  HOPTIME, neighbourList[j]));
								//NodeStatus[neighbourList[j]] = HOPPING;
								Event* hopEvent = (Event*)malloc(sizeof(Event));
								hopEvent->time = current + HOPTIME;
								hopEvent->type = HOP;
								hopEvent->subject = event->object;
								hopEvent->object = neighbourList[j];
								hopEvent->keyRound = event->keyRound;
								insertNode(&EventHeap, hopEvent);

								addToActiveList(hopEvent);
								break;
							}
							else	temp--;
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
			LOGPRINT(("%f second should be detected\n", current+detectionTime));

			Event* detectEvent = (Event*)malloc(sizeof(Event));
			detectEvent->time = current + detectionTime;
			detectEvent->type = DETECTION;
			// subject should be DCU, which is supposed to be at node 0
			detectEvent->subject = 0;
			detectEvent->object = event->object;
			detectEvent->keyRound = event->keyRound;
			insertNode(&EventHeap, detectEvent);

			addToActiveList(detectEvent);
			if (eventTime < detectionTime)	eventTime = detectionTime;

			break;
		}
		case DETECTION:		{
			
			if (event->object == 0)
			{
				printf("show me!\n");
				exit(-1);
			}


			LOGPRINT(("at %f, node %d is detected\n", current, event->object));
			
			// set node to detected
			NodeStatus[event->object] = DETECTED;

			// disconnect its neighbour
			
			keyRound++;

			if (KeyChain[event->subject]  < event->keyRound)	
			{
				printf("impossible!\n");
				exit(-1);
			}
			NodeStatus[event->subject] = PROTECTED;
			KeyChain[event->subject] = keyRound; 

			disconnect(event->object);
			// new attack detected, generate new key and distribute
			int i;
			int* dcuNeighbour = CopyGraph[event->subject];		//event->subject == 0 
			for (i = 1; dcuNeighbour[i] != -1; i++)
			{
				if (dcuNeighbour[i] != DISCONNECTED && NodeStatus[dcuNeighbour[i]] != DETECTED)
				{	
					LOGPRINT(("send new key to %d\n", dcuNeighbour[i]));
					Event* keyDist = (Event*)malloc(sizeof(Event));
					keyDist->time = current + DISTIME;
					keyDist->type = KEYUPDATE;
					keyDist->subject = 0;
					keyDist->object = dcuNeighbour[i];
					keyDist->keyRound = keyRound;
					insertNode(&EventHeap, keyDist);

					addToActiveList(keyDist);
				}
			}
			break;
		}
		case HOP:	{		
			// if is already compromised, ignore
			if (NodeStatus[event->object] == COMPROMISED || NodeStatus[event->object] == COMPROMISING || NodeStatus[event->object] == HOPPING || NodeStatus[event->object] == DETECTED)		break;

			LOGPRINT(("reach node: %d\n", event->object));
			
			NodeStatus[event->subject] = COMPROMISING;
			//NodeStatus[event->object] = COMPROMISING;

			LOGPRINT(("%f second later compromise node %d\n", COMTIME, event->object));
			Event* compromiseEvent = (Event*)malloc(sizeof(Event));
			compromiseEvent->time = current + COMTIME;
			compromiseEvent->type = COMPROMISE;
			compromiseEvent->subject = event->subject;
			compromiseEvent->object = event->object;
			compromiseEvent->keyRound = event->keyRound;
			insertNode(&EventHeap, compromiseEvent);

			addToActiveList(compromiseEvent);
			eventTime = COMTIME;
			break;
		}
		case KEYUPDATE:	{
			// get a key from others, compare it with itself, out-of-date
			if (event->keyRound <= KeyChain[event->object])		break;

			// it's a new key
			LOGPRINT(("Node %d get a new key from %d\n", event->object, event->subject));

			// if it's acquired by bad buy
			if (NodeStatus[event->object] == COMPROMISED || NodeStatus[event->object] == HOPPING || NodeStatus[event->object] == COMPROMISING)
			{
				LOGPRINT(("Bad guy got the key!\n"));
				// stop old attack, because it's very likely to fail
				clearActiveList(event->object);

				// update status and key
				NodeStatus[event->object] = HOPPING;
				KeyChain[event->object] = event->keyRound;

				// start new attack with the new key
				int* neighbourList = CopyGraph[event->object];
				int i;
				for (i = 1; neighbourList[i] != -1; i++)
				{
					// only attack 'good' node
					if (neighbourList[i] != DISCONNECTED && NodeStatus[neighbourList[i]] != DETECTED && NodeStatus[neighbourList[i]] != COMPROMISED)
					{
						LOGPRINT(("%f second later hop to %d\n", HOPTIME, neighbourList[i]));
						Event* attackWithNewKey = (Event*)malloc(sizeof(Event));
						attackWithNewKey->time = current + HOPTIME;
						attackWithNewKey->type = HOP;
						attackWithNewKey->subject = event->object;
						attackWithNewKey->object = neighbourList[i];
						attackWithNewKey->keyRound = event->keyRound;
						insertNode(&EventHeap, attackWithNewKey);

						addToActiveList(attackWithNewKey);
						
						//NodeStatus[neighbourList[i]] = HOPPING;
					}
				}
				break;
			}

			// good guy get the key
			
			// update its status and keychain
			NodeStatus[event->object] = PROTECTED;
			KeyChain[event->object] = event->keyRound;

			// pass the key around
			int i;
			int* neighbourList = CopyGraph[event->object];
			for (i = 1; neighbourList[i] != -1; i++)
			{
				if (neighbourList[i] != DISCONNECTED && NodeStatus[neighbourList[i]] != DETECTED && neighbourList[i] != event->subject && neighbourList[i] != 0)
				{
					LOGPRINT(("pass this key to %d\n", neighbourList[i]));
					Event* keyDist = (Event*)malloc(sizeof(Event));
					keyDist->time = current + DISTIME;
					keyDist->type = KEYUPDATE;
					keyDist->subject = event->object;
					keyDist->object = neighbourList[i];
					keyDist->keyRound = event->keyRound;
					insertNode(&EventHeap, keyDist);

					addToActiveList(keyDist);
				}
			}
		}
		default:	break;
		}
		HeapDelMin(EventHeap);
	} while(1);
	statistic(current);
	LOGCLOSE();	
	freeall();
}

//argv1: start graph
//argv2: end graph
//argv3: graph directory
//argv4: results file
//argv5: number of starting points to use
//argv6: which model
int main(int args, char** argv)
{
	seed = 42397952317;
/*	whichModel = 2;
	Results = fopen("results.txt", "w");
	fprintf(Results, "%20s%20s%20s%20s%20s%20s%20s%20s\n", "Graph File", "Compromise Time", "Hop Time", "Detect Time", "Ratio", "Start Point", "Num Compromised", "% Alive");
//	runSim("Graph1506/graph0.txt", 1, 2, 2, 1, INT_MAX);

	int compromiseTime[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	int detectTime[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	int hopTime = 2;

	int count = 0;
	for (int j = 0; j < 10; j++)
	{
		for (int k = 0; k < 10; k++)
		{
			for (int i = 0; i < 50; i++)
			{
				printf("run sim %d\n", count++);
				runSim("Graph5010/graph0.txt", i, compromiseTime[j], hopTime, detectTime[k], INT_MAX);
			}		
		}
	}
*/

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
	
	whichModel = atoi(argv[6]);
	
	if (whichModel != 1 && whichModel != 2)
	{
		printf("choose from model 1 or 2\n");
		exit(-1);
	}
	
	int numOfNodesToCom = INT_MAX;
	
	int count = 0;
	//graphs
	
	for(int i = startGraphNum; i <= lastGraphNum; i++)
	{
		char graph[30];
		sprintf(graph, "%s/graph%d", argv[3], i);
		strcat(graph, ".txt");
		printf("%s\n", graph);

		for (int j = 0; j < atoi(argv[5]); j++)
		{
			printf("running sim %d\n", count++);
			if (whichModel == 1)
				runSim1(graph, j, compromiseTime, hopTime, detectTime, numOfNodesToCom);
			else
				runSim2(graph, j, compromiseTime, hopTime, detectTime, numOfNodesToCom);
		}
	}
	fclose(Results);
	return 0;
}
