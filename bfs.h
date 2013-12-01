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
void bfsPrint(int *bfs);
void myprint(int** graph);
#endif



