#ifndef ATTACKSIM_H
#define ATTACKSIM_H

#include "Heap.h"

#define SIMTIME		50.0	
#define COMTIME		5.0			//TIME TO COMPROMISE
#define DETTIME		2.0		//TIME TO DETECT
#define RECTIME		5.0			//TIME TO RECOVER
#define HOPTIME		5.0			//TIME TO HOP

//flag adjlist
#define END_OF_LIST				-1
#define DISCONNECTED			-2

//node status
#define GOOD			0
#define COMPROMISED		1
#define HOPED			2
#define DETECTED		3

#endif
