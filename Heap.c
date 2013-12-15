#include <stdlib.h>
#include <stdio.h>
#include "Heap.h"

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
	Event* temp = HeapGetMin(heap);
	event->time = temp->time;
	event->type = temp->type;
	event->subject = temp->subject;
	event->object = temp->object;
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
		newHeap->currentSize++;
	}

	for (i = 0; i < heap->currentSize; i++)
	{
		Event* temp = (heap->array)[i];
		free(temp);
	}
	free(heap);

	return newHeap;
}

