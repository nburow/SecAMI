#include <stdlib.h>
#include <math.h>
#include "random.h"

double power(double *seed, double alpha, int min, int max)
{
	return pow((pow(max, alpha+1)-pow(min, alpha+1))*uniform(seed)+pow(min, alpha+1), 1/(alpha+1));
}
