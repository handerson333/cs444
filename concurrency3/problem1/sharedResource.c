// /*
// Robert Hayden Anderson
// CS444 Operating Systems 2
// Prof: Kevin McGrath
// Concurrency assignment 3 problem 1 
// sources:
// http://pages.cs.wisc.edu/~remzi/Classes/537/Fall2008/Notes/threads-semaphores.txt
// https://stackoverflow.com/questions/14888027/mutex-lock-threads
// http://man7.org/linux/man-pages/man3/pthread_create.3.html
// http://man7.org/linux/man-pages/man3/pthread_join.3.html
// http://greenteapress.com/semaphores/LittleBookOfSemaphores.pdf
// https://oregonstate.instructure.com/courses/1671300/assignments/7246802#submit
// */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include "rng.h"
#define THREADS 10

// Thread stuff
pthread_t threads[THREADS];
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  cond = PTHREAD_COND_INITIALIZER;
// Global for amount of resources in use
int resource = 0;

// functions
void* use_resource(void* void_thread);


int main() {
	printf("\nCurrently using %d threads\n\n", THREADS);
	// create threads
	for(int i=0; i<THREADS; i++){
		pthread_create(&threads[i], NULL, use_resource, i);
	}
		pthread_join(threads[0], NULL);
	

	return 0;		
}

// Use the resource

void* use_resource(void* void_thread){\
	// convert thread number back
	int thread = (int*) void_thread;
	int sleep_time = 0;
	
	//loop forever
	while(1) {
		// try mutex lock
		if(pthread_mutex_trylock(&mutex) == 0) {
			//if 3 resources in use, wait until signal to unlock
			if (resource == 3){
				printf("\nResource is locked\n\n");
				while( resource != 0 ) {
				sleep(3);
				pthread_cond_wait(&cond, &mutex);
				pthread_mutex_unlock(&mutex);
				} 
			}
			// If less than 3 resources in use
			if( resource < 3 ) {
				// get sleep time
				sleep_time = rng(1,10);		
				// add 1 to amount using resource
				resource++;
				printf("Thread %d is using the resource and sleeping for %d seconds\nResources currently in use: %d\n", thread, sleep_time, resource);
				//unlock mutex
				pthread_mutex_unlock(&mutex);
				// sleep while "using the resource"
				sleep(sleep_time);
				//lock mutex
				pthread_mutex_lock(&mutex);
				// no longer using resource, decrement resource
				resource--;
				printf("Thread %d is stopped using the resource\nResources currently in use: %d\n", thread, sleep_time, resource);
				//if no reources in use then send signal to locked loop
				if(resource == 0){
					pthread_cond_signal(&cond);
				}
				pthread_mutex_unlock(&mutex);
			}
		}
		// sleep to let user be able to see whats going on. It gets fast
		sleep(rng(1,3));
	}
}

