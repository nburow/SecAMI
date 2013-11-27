#ifndef _Queue
#define  _Queue
void enqueue(void *val, int size); 
void *dequeue();
int isEmpty();
int length();

typedef struct node{
	void *val;
	struct node *next;
}Node;

#endif
