/*
 * matrix-topology.c
 *
 *  Created on: Nov 20, 2013
 *      Author: nburow
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "random.h"

int **matrix;  //a grid of locations where a node in the mesh network can reside
int size_x = 10;
int size_y = 10;

int main(int argc, char **argv)
{
	matrix = (int **)malloc(size_x*sizeof(int *));
	int i;
	for(i = 0; i < size_x; i++)
		matrix[i] = (int *)malloc(size_y*sizeof(int));



	int j;
	int k;
	for(j = 0; j < size_x; j++)
	{
		for(k = 0; k < size_y; k++)
			printf("%c ", matrix[j][k]);
		printf("\n");
	}
	return EXIT_SUCCESS;
}
