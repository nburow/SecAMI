#ifndef EVENT_H
#define EVENT_H

enum EventType {COMPROMISE, DETECTION, HOP, NOEVENT};

typedef struct
{
	double time;
	enum EventType type;
	int subject;
} Event;

#endif
