/*
============================================================================
Filename    : integral.c
Author      : Jean Knubel
SCIPER		: 328774
============================================================================
*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "utility.h"
#include "function.c"

double integrate (int num_threads, int samples, int a, int b, double (*f)(double));

int main (int argc, const char *argv[]) {
    //test
    int num_threads, num_samples, a, b;
    double integral;

    if (argc != 5) {
		printf("Invalid input! Usage: ./integral <num_threads> <num_samples> <a> <b>\n");
		return 1;
	} else {
        num_threads = atoi(argv[1]);
        num_samples = atoi(argv[2]);
        a = atoi(argv[3]);
        b = atoi(argv[4]);
	}

    set_clock();

    /* You can use your self-defined funtions by replacing identity_f. */
    integral = integrate (num_threads, num_samples, a, b, identity_f);

    printf("- Using %d threads: integral on [%d,%d] = %.15g computed in %.4gs.\n", num_threads, a, b, integral, elapsed_time());

    return 0;
}


double integrate (int num_threads, int samples, int a, int b, double (*f)(double)) {

    //step 1: check variables and split the difference between each evaluated point
    assert(samples > 0);
    assert(num_threads >= 1);
    assert(f != NULL);

    if(a == b){ return 0;}



    /* Your code goes here */
    double delta = abs(b - a) / (double)samples;
    double sum;
    double integral = 0;

    omp_set_num_threads(num_threads);
    
    #pragma omp parallel private(sum)
    {
        sum = 0;
        #pragma omp for
        for(int i = 0; i<samples; i++){
            sum += f(a + i*delta) * (b - a);
        }
    
        #pragma omp atomic 
        integral += sum;
    }

    return integral / samples;
}
