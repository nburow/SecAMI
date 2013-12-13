#ifndef ATTACKSIM_H
#define ATTACKSIM_H

#include "Heap.h"

#define WRONG		65536
#define NOONE		-3

#define SIMTIME		50.0	
/*
#define COMTIME		3.0			//TIME TO COMPROMISE
#define DETTIME		2.0			//TIME TO DETECT
#define RECTIME		5.0			//TIME TO RECOVER
#define HOPTIME		2.0			//TIME TO HOP
*/

//flag adjlist
#define END_OF_LIST				-1
#define DISCONNECTED			-2

//node status
#define GOOD			0
#define COMPROMISED		1
//#define ARRIVED		2
#define DETECTED		3
#define HOPPING			4
#define COMPROMISING	5
//#define ATTACKING		6
#define DETECTING		7

typedef struct activeNode
{
	Event* event;
	struct activeNode* next;
	Boolean active;
} ActiveNode;

typedef ActiveNode* ActiveNodePtr;
typedef ActiveNodePtr* ActiveEventList;

void addToActiveList(Event* event);

#endif
