/*
============================================================================
Filename    : pi.c
Author      : Your names goes here
SCIPER		: Your SCIPER numbers
============================================================================
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
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

typedef struct {
    int chunk;
    int count;
    int thread_num;
} pithread;

void* thread_routine(void *thread_struct) {
    double x, y;
    rand_gen gen = init_rand_pthreads(((pithread *)thread_struct)->thread_num);
    for (int i = 0; i < ((pithread *)thread_struct)->chunk; i++) {
        x = next_rand(gen);
        y = next_rand(gen);
        ((pithread *)thread_struct)->count += ((x*x + y*y) < 1);
        printf("count = %d, (tid) = %d\n", ((pithread *)thread_struct)->count,
                                           ((pithread *)thread_struct)->thread_num);
    }
    free_rand(gen);
    return NULL;
}


double calculate_pi (int num_threads, int samples) {
    double pi;
    int chunk = samples / num_threads;
    int remainder = samples - chunk * num_threads;

    pithread container[num_threads];
    pthread_t tid[num_threads];

    for(int i = 0; i < num_threads; i++){
        container[i].count = 0;
        container[i].thread_num = i;
        container[i].chunk = chunk;
        pthread_create(&tid[i], NULL, thread_routine, &container[i]);
    }
    int accum = 0;
    for(int i = 0; i < num_threads; i++){
        pthread_join(tid[i], NULL);
        accum += (container[i]).count;
    }

    pi = 4*( ((double)accum)/samples );
    return pi;
}
