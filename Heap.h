#ifndef HEAP_H
#define HEAP_H

#include "Event.h" 
#define HEAPSIZE	1000

struct myHeap{
	Event** array;
	int maxSize;
	int currentSize;
};

typedef struct myHeap* Heap;
typedef Heap* HeapPointer; 

void MinHeapBuild(Heap heap);
void Heapify(Heap heap, int currentNode);
void HeapPop(Heap heap, Event* event);
Heap HeapIncrement(Heap heap);
Event* getMin(Heap heap);
void delMin(Heap heap);
void insertNode(HeapPointer hp, Event* event);

#endif
