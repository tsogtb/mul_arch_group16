/*
============================================================================
Filename    : sharing.c
Author      : Tsogt Baigalmaa
SCIPER		: 345120
============================================================================
*/

#include <stdio.h>
#include <stdlib.h>
#include "utility.h"

int perform_buckets_computation(int, int, int);

int main (int argc, const char *argv[]) {
    int num_threads, num_samples, num_buckets;

    if (argc != 4) {
		printf("Invalid input! Usage: ./sharing <num_threads> <num_samples> <num_buckets> \n");
		return 1;
	} else {
        num_threads = atoi(argv[1]);
        num_samples = atoi(argv[2]);
        num_buckets = atoi(argv[3]);
	}
    
    set_clock();
    perform_buckets_computation(num_threads, num_samples, num_buckets);

    printf("Using %d threads: %d operations completed in %.4gs.\n", num_threads, num_samples, elapsed_time());
    return 0;
}

int perform_buckets_computation(int num_threads, int num_samples, int num_buckets) {
    //                      (untouched version)
    /*       
    volatile int *histogram = (int*) calloc(num_buckets, sizeof(int));
    rand_gen generator = init_rand();
    for(int i = 0; i < num_samples; i++){
        int val = next_rand(generator) * num_buckets;
        histogram[val]++;
    }
    free_rand(generator);
    */

    
    // Naive implementation with no optimization
    /*
    int *histogram = (int*) calloc(num_buckets, sizeof(int));
    int val;
    omp_set_num_threads(num_threads);
    #pragma omp parallel private(val) shared(histogram)
    {
        rand_gen generator = init_rand();
        val = 0;
        #pragma omp for schedule(static) nowait
            for(int i = 0; i < num_samples; i++) {
                val = next_rand(generator) * num_buckets;
                #pragma omp atomic 
                histogram[val]++; 
            }
        free_rand(generator);
    }    
    */

    // Variant 1: private arrays then merge to reduce true sharing 
    // with no critical sections
    
    /*
    int *histogram = (int*) calloc(num_buckets, sizeof(int));
    int **temp_hist = (int**) calloc(num_threads, sizeof(int*));
    for(int j = 0; j < num_threads; j++) {
        temp_hist[j] = calloc(num_buckets, sizeof(int));
    }
    int val;
    omp_set_num_threads(num_threads);
    #pragma omp parallel private(val) shared(temp_hist)
    {
        rand_gen generator = init_rand();
        val = 0;
        #pragma omp for schedule(static) nowait
            for(int i = 0; i < num_samples; i++) {
                val = next_rand(generator) * num_buckets;
                int tid = omp_get_thread_num();
                temp_hist[tid][val]++;
            }
        free_rand(generator);
    } 
    #pragma omp parallel shared(temp_hist, histogram)
    {
        #pragma omp for schedule(static) nowait
            for(int i = 0; i < num_buckets; i++) {
                int tid;
                for (tid = 0; tid < num_threads; ++tid) {
                    histogram[i] += temp_hist[tid][i];
                }
            }
    }
    */

    //Variant 2: optimized version with no true/false sharing

    #define BLOCKSIZE 64
    
    int *histogram = (int*) calloc(num_buckets, BLOCKSIZE);
    int **temp_hist = (int**) calloc(num_threads, BLOCKSIZE);
    for(int j = 0; j < num_threads; j++) {
        temp_hist[j] = calloc(num_buckets, BLOCKSIZE);
    }
    int val;
    omp_set_num_threads(num_threads);
    #pragma omp parallel private(val) shared(temp_hist)
    {
        rand_gen generator = init_rand();
        val = 0;
        #pragma omp for schedule(static) nowait
            for(int i = 0; i < num_samples; i++) {
                val = next_rand(generator) * num_buckets;
                int tid = omp_get_thread_num();
                temp_hist[tid][val]++;
            }
        free_rand(generator);
    } 
    #pragma omp parallel shared(temp_hist, histogram)
    {
        #pragma omp for schedule(static) nowait
            for(int i = 0; i < num_buckets; i++) {
                int tid;
                for (tid = 0; tid < num_threads; ++tid) {
                    histogram[i] += temp_hist[tid][i];
                }
            }
    }
    
    return 0;
}
