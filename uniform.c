//Function returns a single U(0,1) variate
//ignore compilation warnings

#include <stdio.h>
#include <math.h>


double uniform(double *dseed) {
	double d2p31m = 2147483647,
	d2p31  = 2147483711;

	*dseed = 16807*(*dseed) - floor(16807*(*dseed)/d2p31m) * d2p31m;
	return( fabs((*dseed / d2p31)) ); 
}
