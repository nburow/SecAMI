/*
 * bfs.h
 *
 *  Created on: Dec 1, 2013
 *      Author: nburow
 */

#ifndef _BFS
#define _BFS

#include <stdio.h>

//must call getGraph before bfs
int *bfs(int start, int **graph);

//call before bfs
int **getGraph(FILE *in);

//call getGraph before distance;
int distance(int start, int end, int **graph);

void bfsPrint(int *bfs);
void myprint(int** graph);
#endif



