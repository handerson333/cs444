/*
Robert Hayden Anderson
CS444 Operating Systems 2
Prof: Kevin McGrath
Concurrency assignment 2


sources:
http://pages.cs.wisc.edu/~remzi/Classes/537/Fall2008/Notes/threads-semaphores.txt
https://stackoverflow.com/questions/14888027/mutex-lock-threads
http://man7.org/linux/man-pages/man3/pthread_create.3.html
http://man7.org/linux/man-pages/man3/pthread_join.3.html
http://greenteapress.com/semaphores/LittleBookOfSemaphores.pdf
https://oregonstate.instructure.com/courses/1671300/assignments/7246802#submit
*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <time.h>
#include "rng.h"

void* func(void* person);
void* philosopher_thread(void* philosopher_struct);

pthread_t seat[5];
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t fork_0 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t fork_1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t fork_2 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t fork_3 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t fork_4 = PTHREAD_MUTEX_INITIALIZER;
typedef struct philosopher
{
    char name[20];
    pthread_mutex_t fork_left_mutex;
    pthread_mutex_t fork_right_mutex;
    int fork_left_number;
    int fork_right_number;
    void (*func)(struct philosopher person);
}philosopher;
int main() 
{
    philosopher einstein = {"Einstein ", fork_4, fork_0, 4, 0, func};
    philosopher aristotle = {"Aristotle", fork_0, fork_1, 0, 1, func};
    philosopher plato = {"Plato    ", fork_1, fork_2, 1, 2, func};
    philosopher socrates = {"Socrates ", fork_2, fork_3, 2, 3, func};
    philosopher marx = {"Karl Marx", fork_3, fork_4, 3, 4, func};
    philosopher *all[5] = {&einstein, &aristotle, &plato, &socrates, &marx};
    for (int i = 0; i < 5; i++){
        pthread_create(&seat[i], NULL, func, all[i]);
    }
    for (int i = 0; i < 5; i++){
        pthread_join(seat[i], NULL);
    }
	return 0;
}

void* func(void* person)
{
    philosopher* ph = (struct philosopher*)person;
    int x = 0;
	int think = 0;
	while(1) {
		if(!think){
			while(1) {
				if(pthread_mutex_trylock(&lock) == 0) {
					x = rng(1);
					pthread_mutex_unlock(&lock);
					printf("%sis thinking for %d seconds\n", ph->name, x);
					sleep(x);
					break;
				}	
			}
		}
		if(pthread_mutex_trylock(&ph->fork_left_mutex) == 0) {
				printf("%s picks up fork %d\n", ph->name, ph->fork_left_number );
			if(pthread_mutex_trylock(&ph->fork_right_mutex) == 0) {
				printf("%s picks up fork %d\n", ph->name, ph->fork_right_number );
				if(pthread_mutex_trylock(&lock) == 0) {
					x = rng(0);
					pthread_mutex_unlock(&lock);
					printf("%s is eating for %d seconds\n", ph->name, x);
					sleep(x);
					think= 0;
					printf("%s puts down fork %d\n", ph->name, ph->fork_left_number );
					printf("%s puts down fork %d\n", ph->name, ph->fork_right_number );
                    pthread_mutex_unlock(&ph->fork_left_mutex);
					pthread_mutex_unlock(&ph->fork_right_mutex);
				}
			} else {
				think = 1;
				printf("%s puts down fork %d\n", ph->name, ph->fork_left_number);
				printf("%s puts down fork %d\n", ph->name, ph->fork_right_number );
                pthread_mutex_unlock(&ph->fork_left_mutex);
				pthread_mutex_unlock(&ph->fork_right_mutex);
			}
		} else {
			think = 1;	
			printf("%s puts down fork %d\n", ph->name, ph->fork_left_number );
            pthread_mutex_unlock(&ph->fork_left_mutex);
		}
	}
}
