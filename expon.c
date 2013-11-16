
// Function returns a single Exponential variate with mean xm
// Function Expon requires Uniform.c


#include <stdio.h>
#include <math.h>

double uniform (double *dseed);

double expon(double *dseed, float xm) {
	return( (-(xm) * log((double)uniform(dseed))) );
}
