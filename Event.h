#ifndef EVENT_H
#define EVENT_H

enum EventType {COMPROMISE, DETECTION, HOP, KEYUPDATE, NOEVENT};
enum BOOLEAN {FALSE, TRUE};

typedef enum BOOLEAN Boolean;
typedef struct
{
	double time;
	enum EventType type;
	int subject;
	int object;
	int keyRound;
} Event;

#endif
