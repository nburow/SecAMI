/*
 * mulvalInput.c
 *
 *  Created on: Nov 17, 2013
 *      Author: nburow
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FILE *out;
FILE *in;

int size = 100;

char *getLine()
{
	char *line = malloc(size*sizeof(char));
	memset(line, 0, size*sizeof(char));

	char c;
	int i = 0;
	while((c = getc(in)) != EOF && c != '\n')
	{
		line[i++] = c;
		if(i > size - 2) //index: -1, space for '\0' terminator: -2
		{
			size *= 2;
			realloc(line, size*sizeof(char));
		}
	}
	line[i] = '\0';
	return line;
}
void writeConnections()
{
	char *line = getLine();
	while(strlen(line) > 0)
	{
		printf("%s\n", line);
		free(line);
		line = getLine();
	}
}
//arguments: name of input file.
int main(int argc, char** argv)
{
	out = fopen(argv[1], "w");
	in = fopen("network.txt", "r");

	//connections
	writeConnections();

	//comments for node setup

	fclose(out);
	fclose(in);
	return EXIT_SUCCESS;
}

