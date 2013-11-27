#ifndef RND
#define RND

double uniform (double *dseed);

double expon(double *dseed, float xm);

double power_rng(double *seed, double alpha, int min, int max);

#endif 
