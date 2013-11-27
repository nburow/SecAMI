#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "queue.h"

Node *head = NULL;
Node *tail = NULL;
int l = 0;

int length()
{
	return l;
}

void enqueue(void *val, int size)
{
	l++;
	void *tmp = malloc(size);
	memcpy(tmp,val,size);

	Node *next = (Node *)malloc(sizeof(Node));
	next->val = tmp;
	next->next = NULL;

	if(head == NULL)
	{
		head = next;
		tail = next;
	}
	else
	{
		tail->next = next;
		tail = next;
	}
}

void *dequeue()
{
	void *result;

	l--;
	if(head == NULL)
		result =  NULL;
	else 
	{
		result = head->val;
		head = head->next;
		if(head == NULL)
			tail = NULL;
	}
	return result;
}

int isEmpty()
{
	return (l == 0) ? 1 : 0;
}
