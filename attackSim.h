#ifndef ATTACKSIM_H
#define ATTACKSIM_H

#include "Heap.h"

//flag adjlist
#define END_OF_LIST				-1
#define DISCONNECTED			-2

//node status
#define GOOD			0
#define COMPROMISED		1
#define DETECTED		3
#define HOPPING			4
#define COMPROMISING	5
#define DETECTING		7
#define PROTECTED		8

//initial key
#define KEYZERO			0


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
