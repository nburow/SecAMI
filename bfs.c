/*
 * bfs.c
 *
 *  Created on: Nov 30, 2013
 *      Author: nburow
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

FILE *in;

int size = 100;

char *getLine()
{
	char *line = (char *)malloc(size*sizeof(char));
	memset(line, 0, size*sizeof(char));

	char c;
	int i = 0;
	while((c = getc(in)) != EOF && c != '\n')
	{
		line[i++] = c;
		if(i > size - 2) //index: -1, space for '\0' terminator: -2
		{
			size *= 2;
			void *x = realloc(line, size*sizeof(char));
		}
	}
	line[i] = '\0';
	return line;
}
char *parseWord(char **tmp)
{
	char *word = (char *)malloc(size*sizeof(char));
	memset(word, 0, size*sizeof(char));

	int i = 0;
	int j = 0;

	while(isblank((*tmp)[j]) || ispunct((*tmp)[j]))
		j++;

	while(isalnum((*tmp)[j]))
	{
		word[i] = (*tmp)[j];
		i++;
		j++;
	}
	word[i] = '\0';
	*tmp = *tmp + j;

	return word;
}

//arguments: name of input file.
int main(int argc, char** argv)
{
	in = fopen(argv[1], "r");


	fclose(in);
	return EXIT_SUCCESS;
}


