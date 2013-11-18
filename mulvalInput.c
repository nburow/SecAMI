/*
 * mulvalInput.c
 *
 *  Created on: Nov 17, 2013
 *      Author: nburow
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

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
char *parseWord(char **tmp)
{
	char *word = malloc(size*sizeof(char));
	memset(word, 0, size*sizeof(char));

	int i = 0;
	int j = 0;

	while(isblank(*tmp[j]) || ispunct(*tmp[j]))
		j++;

	printf("%c\n", *tmp[j]);

	while(isdigit(*tmp[j]))
	{
		word[i] = *tmp[j];
		i++;
		j++;
	}
	word[i] = '\0';
	*tmp = *tmp + i;
	//printf("%s\n", tmp);
	return word;
}
void writeConnections()
{
	//set-up
	fprintf(out, "/*Attacker located*/\n");
	fprintf(out, "/*Attacker goal*/\n\n\n");
	fprintf(out, "/*Connections*/\n");

	//write connections
	char *line = getLine();
	char *holder = line;
	//printf("%s\n", tmp);
	while(strlen(line) > 0)
	{
		//printf("%s\n", line);
		char *word = parseWord(&line);
		while(strlen(word) > 0)
		{
			fprintf(out, "%s ", word);
			word = parseWord(&line);
		}
		fprintf(out, "\n");
		free(holder);
		line = getLine();
		holder = line;
	}
}
//arguments: name of output file.
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

