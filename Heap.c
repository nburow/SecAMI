#include <stdlib.h>
#include <stdio.h>
#include "Heap.h"

int countFree = 0;

//void iniFreeCount()	{countFree = 0;}

int getFreeCount()
{
	return countFree;
}

void swap(Event* array[], int a, int b)
{
	Event* temp = array[a];
	array[a] = array[b];
	array[b] = temp;
}

void MinHeapBuild(Heap heap)
{
	int i;
	for(i = (heap->currentSize)/2-1; i>= 0; i--)	
	{
		Heapify(heap, i);
	}
}

void Heapify(Heap heap, int currentNode)
{
	int leftChild = currentNode*2+1;
	int rightChild = currentNode*2+2;
	int min = currentNode;
	if(leftChild < heap->currentSize && (heap->array)[leftChild]->time < (heap->array)[currentNode]->time)
	{
		min = leftChild;
	}
	if (rightChild < heap->currentSize && (heap->array)[rightChild]->time <  (heap->array)[min]->time)
	{
		min = rightChild;
	}
	if (min != currentNode)
	{
		swap(heap->array, currentNode, min);
		Heapify(heap, min);
	}
}

Event* HeapGetMin(Heap heap)
{
	if (HeapIsEmpty(heap))		return NULL;
	return (heap->array)[0];
}

void HeapDelMin(Heap heap)
{
	free((heap->array)[0]);
	(heap->array)[0] = (heap->array)[heap->currentSize-1];
	Heapify(heap, 0);
	heap->currentSize--;
}

void insertNode(HeapPointer hp, Event* event)
{
	Heap heap = *hp;
	if (heap->currentSize == heap->maxSize)
	{
		heap = HeapIncrement(heap);
	}
	(heap->array)[heap->currentSize] = event;
	heap->currentSize++;
/*	int i;
	for (i = 0; i < heap->currentSize; i++)		printf("%f ", (heap->array)[i]->time);
	printf("\n%d\n", heap->currentSize);
*/	
	int cIndex = heap->currentSize-1;
	int pIndex = heap->currentSize/2-1;
	while (pIndex >= 0)
	{
		if ((heap->array)[cIndex]->time < (heap->array)[pIndex]->time)
		{
			swap(heap->array, cIndex, pIndex);
			cIndex = pIndex;
			pIndex = (pIndex-1)/2;
		}
		else	break;
	}
	*hp = heap;
}

Boolean HeapIsEmpty(Heap heap)
{
	if (heap->currentSize == 0)		return TRUE;
	return FALSE;
}
void HeapPop(Heap heap, Event* event)
{
/*	if (HeapIsEmpty(heap))	
	{
		event->type = NOEVENT;
		return;
	}
*/	Event* temp = HeapGetMin(heap);
	event->time = temp->time;
	event->type = temp->type;
	event->subject = temp->subject;
	event->object = temp->object;
//	event->active = temp->active;
	HeapDelMin(heap);
}

Heap HeapIncrement(Heap heap)
{
	Heap newHeap = (Heap)malloc(sizeof(Heap));
	newHeap->array = (Event**)malloc(sizeof(Event*) * (heap->currentSize + HEAPSIZE));
	newHeap->maxSize = heap->currentSize + HEAPSIZE;
	newHeap->currentSize = 0;
	int i;
	for (i = 0; i < heap->currentSize; i++)
	{
		(newHeap->array)[i] = (Event*)malloc(sizeof(Event));
		(newHeap->array)[i]->time = (heap->array)[i]->time;
		(newHeap->array)[i]->type = (heap->array)[i]->type;
		(newHeap->array)[i]->subject = (heap->array)[i]->subject;
		(newHeap->array)[i]->object = (heap->array[i])->object;
	//	(newHeap->array)[i]->active = (heap->array[i])->active;
		newHeap->currentSize++;
	}

	for (i = 0; i < heap->currentSize; i++)
	{
		Event* temp = (heap->array)[i];
		free(temp);
	}
	free(heap);
//	free(heap->array);
//	heap->maxSize = 0;
//	heap->currentSize = 0;

	return newHeap;
}

/*
int main()
{
	// test
	Heap heap = (Heap)malloc(sizeof(Heap));
	heap->array = (char*)malloc(sizeof(Event*) * 100);
	heap->maxSize = 100;
	heap->currentSize = 0;

	int i;
	for (i = 0; i < 100; i++)
	{
		Event* newEve = (Event*)malloc(sizeof(Event));
		newEve->time = 100-i;
		newEve->type = 1;
		(heap->array)[heap->currentSize] = newEve;
		heap->currentSize++;
	}

	MinHeapBuild(heap);
	for (i = 0; i < heap->currentSize; i++)		printf("%f ", (heap->array)[i]->time);
	printf("\n%d\n", heap->currentSize);

	for (i = heap->currentSize-1; i >= 0; i--)
	{
		swap(heap->array, 0, i);
		heap->currentSize--;
		Heapify(heap, 0);
	}
	for (i = 0; i < 100; i++)		printf("%f ", (heap->array)[i]->time);
	printf("\n");

	Event* event = (Event*)malloc(sizeof(Event));
	event->time = 30;
	event->type = 2;
	insertNode(&heap, event);
	for (i = 0; i < heap->currentSize; i++)		printf("%f ", (heap->array)[i]->time);
	printf("\n%d\n", heap->currentSize);
	
	Event e;
	while (heap->currentSize > 0)
	{
		HeapPop(heap, &e);
		printf("%f ", e.time);
	}
	printf("\n");

	Event e;
	HeapPop(heap, &e);
	printf("%f\n", (heap->array)[0]->time);
	printf("%f", e.time);
	printf("\n");

	return 0;
}
*/
