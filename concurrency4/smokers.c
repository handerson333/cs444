// /*
// Robert Hayden Anderson
// CS444 Operating Systems 2
// Prof: Kevin McGrath
// Concurrency assignment 4 smokers
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

//semaphores
sem_t general_agent;
sem_t tobacco;
sem_t paper;
sem_t match;
sem_t smoker_tobacco; 
sem_t smoker_paper; 
sem_t smoker_matches; 
//mutex so only one pusher is used at a time
pthread_mutex_t general_pusher;
//bools for resources
int tobacco_in_use = 0;
int paper_in_use = 0;
int match_in_use = 0;
//time for each smoker
int smoke_time;
//func declarations
void smoker_a();
void smoker_c();
void smoker_b();
void agent_c();
void agent_b();
void agent_a();
void pusher_a();
void pusher_c();
void pusher_b();
void sem_setup();

//MAIN
int main(){	
    pthread_t threads[9];
	
    sem_setup();
	pthread_mutex_init(&general_pusher, NULL);
	pthread_create(&threads[0], NULL, smoker_a, NULL);
	pthread_create(&threads[1], NULL, smoker_b, NULL);
	pthread_create(&threads[2], NULL, smoker_c, NULL);
	pthread_create(&threads[3], NULL, pusher_a, NULL);
	pthread_create(&threads[4], NULL, pusher_b, NULL);
	pthread_create(&threads[5], NULL, pusher_c, NULL);
	pthread_create(&threads[6], NULL, agent_a, NULL);
	pthread_create(&threads[7], NULL, agent_b, NULL);
	pthread_create(&threads[8], NULL, agent_c, NULL);
	
	for(int i = 0; i < 9; i++){
		pthread_join(threads[i], NULL);
	}

}


//AGENTS
// each agent just waits for the general agent to be done so they have to wait their turn and then post the resources they have. 
// All agents do the same thing but for the different resources
void agent_a(){
	while(1){
		sem_wait(&general_agent);
		printf("\nAgent A has paper and tobacco ready\n");
		sem_post(&tobacco);
		sem_post(&paper);
	}
}
void agent_b(){
	while(1){
		sem_wait(&general_agent);
		printf("\nAgent B has paper and a match ready\n");
		sem_post(&paper);
		sem_post(&match);
	}
}
void agent_c(){
	while(1){
		sem_wait(&general_agent);
		printf("\nAgent C has tobacco and a match ready\n");
		sem_post(&tobacco);
		sem_post(&match);
	}
}


// SMOKERS
// All smokers are the same code other than the resources they use and their locks.
// each waits for the the smoker to be called from their respective pusher.
// Then tells the general agent it's ready and smokes


void smoker_a(){
	while(1){
		sem_wait(&smoker_matches);
		sem_post(&general_agent);
        printf("Smoker A with matches gets tobacco and paper from agent A.\n");
		printf("Smoker A makes their cigarette.\n");
        smoke_time = rng(1,5);
        printf("Smoker A smokes for %d seconds.\n", smoke_time);
		sleep(smoke_time);
	}
}
void smoker_b(){	
	while(1){
		sem_wait(&smoker_tobacco);
		sem_post(&general_agent);
        printf("Smoker B with tobacco gets a match and paper from agent B.\n");
		printf("Smoker B makes their cigarette.\n");
        smoke_time = rng(1,5);
        printf("Smoker B smokes for %d seconds.\n", smoke_time);
		sleep(smoke_time);
	}
}
void smoker_c(){
	while(1){
		sem_wait(&smoker_paper);
		sem_post(&general_agent);
        printf("Smoker C with paper gets a match and tobacco from agent C.\n");
		printf("Smoker C makes their cigarette.\n");
        smoke_time = rng(1,5);
        printf("Smoker C smokes for %d seconds.\n", smoke_time);
		sleep(smoke_time);
	}
}

// PUSHERS
// all pushers have the same code other than different resources
// If the tobacco or the paper is in use, then they are free to
// give the match  to smoker a and mark that matches are in use
// this principle goes the same fore each pusher with the smoker 
// and resource changed.

void pusher_a(){
	while(1){
		sem_wait(&match);
		pthread_mutex_lock(&general_pusher);
		
		if(tobacco_in_use){
			tobacco_in_use--;
			sem_post(&smoker_paper);
		}
        else if(paper_in_use){
			paper_in_use--;
			sem_post(&smoker_tobacco);
		}
		else{
			match_in_use ++;
		}
		pthread_mutex_unlock(&general_pusher);
	}
}
void pusher_b(){
	while(1){
		sem_wait(&tobacco);
		pthread_mutex_lock(&general_pusher);
		if(match_in_use){
			match_in_use--;
			sem_post(&smoker_paper);
		}
        else if(paper_in_use){
			paper_in_use--;
			sem_post(&smoker_matches);
		}
		else{
			tobacco_in_use++;
		}
		pthread_mutex_unlock(&general_pusher);
	}
}
void pusher_c(){
	while(1){
		sem_wait(&paper);
		pthread_mutex_lock(&general_pusher);
		if(match_in_use){
			match_in_use--;
			sem_post(&smoker_tobacco);
		}
		else if(tobacco_in_use){
			tobacco_in_use--;
			sem_post(&smoker_matches);
		}
		else{
			paper_in_use ++;
		}
		pthread_mutex_unlock(&general_pusher);
	}

}

// setup all semaphores in a function so as to keep main cleaner.
void sem_setup(){
    sem_init(&general_agent, 0, 1);
	sem_init(&smoker_paper, 0, 0);
	sem_init(&smoker_tobacco, 0, 0);
	sem_init(&smoker_matches, 0, 0);
    sem_init(&paper, 0, 0);
    sem_init(&tobacco, 0, 0);
    sem_init(&match, 0, 0);
}

