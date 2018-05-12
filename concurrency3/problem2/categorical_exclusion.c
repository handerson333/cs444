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
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include "rng.h"

// Amount of threads per type of thread
#define THREADS 3
// struct of linked list
typedef struct list{
	int data;
	struct list *next;
}node;

// declare threads and their mutexes
pthread_t search_thread[THREADS];
pthread_t insert_thread[THREADS];
pthread_t delete_thread[THREADS];
pthread_mutex_t search_mutex;
pthread_mutex_t insert_mutex;
pthread_mutex_t delete_mutex;

// Function definitions
void* search(void* void_head);
void* insert(void* void_head);
void* delete(void* void_head);




int main(){
	node *head = malloc(sizeof(struct list));
	head->data = 0;
	head->next = NULL;
	printf("There are %d threads of each type\n", THREADS);

	// create threads
	for(int i=0; i<THREADS; i++){
		pthread_create(&search_thread[i],NULL,search,head);
		pthread_create(&insert_thread[i],NULL,insert,head);
		pthread_create(&delete_thread[i],NULL,delete,head);
	}
	//join threads
	for(int i=0; i<THREADS; i++) {
		pthread_join(search_thread[i], NULL);
		pthread_join(insert_thread[i], NULL);
		pthread_join(delete_thread[i], NULL);
	}
	return 0;
}

// delete node
void* delete(void* void_head){
	// sleep at beginning so you don't instantly delete nothing
	sleep(2);
	node *head = (node*)void_head;
	while(1){
		int value = rng(1,10);
		int result = 0;
		pthread_mutex_lock(&search_mutex);
		pthread_mutex_lock(&insert_mutex);
		pthread_mutex_lock(&delete_mutex);
		node *prev_node = head->next;
		node *next_node;
		if(prev_node){
			next_node = prev_node->next;
			while(next_node && next_node->next) {
				if(next_node->data != value){
					prev_node = next_node;
					next_node = next_node->next;
				} else {
					prev_node->next = next_node->next;
					break;
				}
			}
		}
		printf("Deleting %d\n",value);
		pthread_mutex_unlock(&delete_mutex);
		pthread_mutex_unlock(&insert_mutex);
		pthread_mutex_unlock(&search_mutex);
		sleep(rng(1,20));
	}
}

void* insert(void* void_head){
	node *head = (node*)void_head;
	while(1){
		int value = rng(1,10);
		
		pthread_mutex_lock(&insert_mutex);
		node *current = head;
		while(current->next != NULL){
			current = current->next;
		}
		current->next = malloc(sizeof(node));
		current->next->data = value;
		current->next->next = NULL;
		pthread_mutex_unlock(&insert_mutex);
		printf("Inserted %d\n",value);
		sleep(rng(1,10));
	}
}


void* search(void* void_head){
	// sleep at beginning so you don't instantly search nothing
	sleep(1);
	node *head = (node*)void_head;
	while(1){
		int value = rng(1,10);
		int result = 0;
		pthread_mutex_lock(&search_mutex);
		node *current = head->next;
		while(current != NULL) {
			if(current->data == value)
				result = 1;
			current = current->next;
		}
		pthread_mutex_unlock(&search_mutex);
		if(result == 1){
			printf("Searching for %d ... found!\n",value);
		}else{
			printf("Searching for %d ... not found!\n",value);
		}	
		sleep(rng(1,15));
	}
}