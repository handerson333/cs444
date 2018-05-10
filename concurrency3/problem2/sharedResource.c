// /*
// Robert Hayden Anderson
// CS444 Operating Systems 2
// Prof: Kevin McGrath
// Concurrency assignment 2
// sources:
// http://pages.cs.wisc.edu/~remzi/Classes/537/Fall2008/Notes/threads-semaphores.txt
// https://stackoverflow.com/questions/14888027/mutex-lock-threads
// http://man7.org/linux/man-pages/man3/pthread_create.3.html
// http://man7.org/linux/man-pages/man3/pthread_join.3.html
// http://greenteapress.com/semaphores/LittleBookOfSemaphores.pdf
// https://oregonstate.instructure.com/courses/1671300/assignments/7246802#submit
// */
// #include <stdlib.h>
// #include <stdio.h>
// #include <string.h>
// #include <pthread.h>
// #include <semaphore.h>
// #include <unistd.h>
// #include <time.h>
// #include "rng.h"

// #define TOTAL_THREADS 5
// void* use_resource(void* users);


// pthread_t users[TOTAL_THREADS];
// pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
// pthread_cond_t  cond = PTHREAD_COND_INITIALIZER;

// //main function
// int main() 
// {


//     for (int i = 0; i < TOTAL_THREADS; i++){
//         pthread_create(&users[i], NULL, use_resource, &i);
//     }
//     //join all the threads
//     for (int i = 0; i < TOTAL_THREADS; i++){
//         pthread_join(users[i], NULL);
//     }
// 	return 0;
// }

// void* use_resource(void * temp){
//     int threads = (int *)threads;
//     int capacity = 3, i = 0;
//     while(1){
        
//         if(pthread_mutex_trylock(&lock) == 0) {

// 			while(threads == 3) {
// 				pthread_cond_wait(&cond, &lock);
// 				printf("\t%s\n","FULL");
// 				pthread_mutex_unlock(&lock);
// 			} 
			
// 			if(threads < 3) {
// 				printf("%s %i \t %s%i\n","threads:",threads,"id:",threads);
// 				threads++;
				
				
// 				pthread_mutex_unlock(&lock);
// 				sleep(rng(5,9));

// 				pthread_mutex_lock(&lock);
// 				threads--;
// 				if(threads == 0)
// 					pthread_cond_signal(&cond);
// 				pthread_mutex_unlock(&lock);
// 			}
// 		}
        
//     }
    



// }


// // //the entire program really...

// // void* philosopher_thread(void* person)
// // {
// //     //recast the struct back to a struct
// //     philosopher* ph = (struct philosopher*)person;
// //     int x = 0;
// // 	int think = 0;
// // 	while(1) {
// //         //if not thinking
// // 		if(!think){
// // 			while(1) {
// //                 //continuously try to get a lock
// // 				if(pthread_mutex_trylock(&lock) == 0) {
// // 					x = rng(1);
// // 					pthread_mutex_unlock(&lock);
// // 					printf("%s is thinking for %d seconds\n", ph->name, x);
// // 					sleep(x);
// // 					break;
// // 				}	
// // 			}
// // 		}
// //         //try to pick up left fork
// // 		if(pthread_mutex_trylock(&ph->fork_left_mutex) == 0) {
// // 				printf("%s picks up fork %d\n", ph->name, ph->fork_left_number );
// //             //if you can pick up left, try to pick up right fork
// // 			if(pthread_mutex_trylock(&ph->fork_right_mutex) == 0) {
// //                 //can pick up bothforks
// // 				printf("%s picks up fork %d\n", ph->name, ph->fork_right_number );
// // 				if(pthread_mutex_trylock(&lock) == 0) {
// // 					x = rng(0);
// // 					pthread_mutex_unlock(&lock);
// // 					printf("%s is eating for %d seconds\n", ph->name, x);
// // 					sleep(x);
// // 					think= 0;
// // 					printf("%s puts down fork %d\n", ph->name, ph->fork_left_number );
// // 					printf("%s puts down fork %d\n", ph->name, ph->fork_right_number );
// //                     pthread_mutex_unlock(&ph->fork_left_mutex);
// // 					pthread_mutex_unlock(&ph->fork_right_mutex);
// // 				}
// // 			} 
// //             //If youre done eating, put down both forks
// //             else {
// // 				think = 1;
// // 				printf("%s puts down fork %d\n", ph->name, ph->fork_left_number);
// // 				printf("%s puts down fork %d\n", ph->name, ph->fork_right_number );
// //                 pthread_mutex_unlock(&ph->fork_left_mutex);
// // 				pthread_mutex_unlock(&ph->fork_right_mutex);
// // 			}
// //         }
// //         //put the left fork back down if you can't pick up both forks 
// //         else {
// // 			think = 1;	
// // 			printf("%s puts down fork %d\n", ph->name, ph->fork_left_number );
// //             pthread_mutex_unlock(&ph->fork_left_mutex);
// // 		}
// // 	}
// // }

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <unistd.h>
#include "rng.h"
#define THREADS 12

void* use_resource(void* arg);

pthread_t threads[THREADS];
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  cond = PTHREAD_COND_INITIALIZER;
int shared = 0;
bool waiting = false;


int main() 
{

	for(int i=0; i<THREADS; i++)
		pthread_create(&threads[i], NULL, use_resource, &i);
	
	pthread_join(threads[0], NULL);

	return 0;		
}

void* use_resource(void* temp)
{
	int thread = *((int *) temp);
	int w_time = 0;

	while(1) {

		sleep(rng(1,2));
		if(pthread_mutex_trylock(&mutex) == 0) {

			while(shared == 3) {
				pthread_cond_wait(&cond, &mutex);
				printf("\t%s\n","FULL");
				pthread_mutex_unlock(&mutex);
			} 
			
			if(shared < 3) {
				printf("%s %i \t%s%i \t%s%i\n","SHARED:",shared,"sleep:",w_time,"thread:",thread);
				shared++;
				w_time = rng(1,2);
				
				pthread_mutex_unlock(&mutex);
				sleep(w_time);

				pthread_mutex_lock(&mutex);
				shared--;
				if(shared == 0)
					pthread_cond_signal(&cond);
				pthread_mutex_unlock(&mutex);
			}
		}
	}
}

