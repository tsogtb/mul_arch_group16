/*
============================================================================
Filename    : pi.c
Author      : Your names goes here
SCIPER		: Your SCIPER numbers
============================================================================
*/

#include <stdio.h>
#include <stdlib.h>
#include "utility.h"

double calculate_pi (int num_threads, int samples);

int main (int argc, const char *argv[]) {

    int num_threads, num_samples;
    double pi;

    if (argc != 3) {
		printf("Invalid input! Usage: ./pi <num_threads> <num_samples> \n");
		return 1;
	} else {
        num_threads = atoi(argv[1]);
        num_samples = atoi(argv[2]);
	}

    set_clock();
    pi = calculate_pi (num_threads, num_samples);

    printf("- Using %d threads: pi = %.15g computed in %.4gs.\n", num_threads, pi, elapsed_time());

    return 0;
}


double calculate_pi (int num_threads, int samples) {
    assert(samples > 0);
    assert(num_threads > 0);
    double pi;
    double x, y;
    int i;
    int pcount = 0;
    int count = 0;
    omp_set_num_threads(num_threads);
    #pragma omp parallel private(x, y) firstprivate(pcount) shared(count) 
    {   
        pcount = 0;
        rand_gen gen = init_rand();
        //statically scheduled parallel for (i is taken care of internally by openmp)
        #pragma omp for schedule(static) nowait 
            for(i = 0; i < samples; i++) {
                x = next_rand(gen);
                y = next_rand(gen);
                pcount += ((x*x + y*y) < 1);
            } 
        //critical
        #pragma omp atomic
        count += pcount;
    } //end of parallel section

    pi = 4*(((double)count)/samples);

    return pi;
}
