#include <stdlib.h>
#include <stdio.h>
#include "random.h"

int main()
{
	double seed = 123456789;
	double alpha = -2.5;
	int min = 1;
	int max = 10;

	int i;
	for(i = 0; i < 10000; i++)
	{
		double x = power(&seed, alpha, min, max);
		printf("%f\n",x);
	}
}
