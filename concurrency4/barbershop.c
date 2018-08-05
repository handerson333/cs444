// /*
// Robert Hayden Anderson
// CS444 Operating Systems 2
// Prof: Kevin McGrath
// Concurrency assignment 4 barbershop
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
#define chairs 5


//globals
int customer_num = 0;
int num_customers = 0;
int open_chairs;
int haircut_duration;
//semaphores
sem_t barber;
sem_t barber_chair;
sem_t begin_haircut;
sem_t customer_sem;
sem_t empty;
//functions
void barber_thread();
void customer_thread();
void customer(int customer_num);
void sem_setup();

// MAIN
int main(){
    sem_setup();
    // set how long haircuts take, between 3 and 10 seconds depending on the cut
	haircut_duration = rng(3,10);
	pthread_t barber;
	pthread_t customers[chairs + 3];
    printf("\nThere are %d waiting chairs in the barbershop\n\n", chairs);
	pthread_create(&barber, NULL, barber_thread, NULL);
	for(int i = 0; i < chairs + 3; i++){
		pthread_create(&customers[i], NULL, customer_thread, NULL);
	}

	pthread_join(barber, NULL);
	for(int i = 0; i < chairs + 3; i++){
		pthread_join(customers[i], NULL);
	}
}

// start a customer, make a number for each customer
void customer_thread(){	
	while(1){
		sleep(rng(1,10));
		sem_wait(&customer_sem);
		customer_num++;
		sem_post(&customer_sem);
        customer(customer_num);
		
    }
}

// Each thread for the customers of the barbershop.
// Check if there are any open chairs available in the shop
// Increase customers, sit down in an open seat and wait for their haircut
// Get their haircut and decrease amount of customers
void customer(int customer_num){
    
	sem_getvalue(&empty, &open_chairs);
    if(num_customers == 0){
        num_customers++;
        sem_post(&barber);
        printf("Customer %d wakes up barber\n",customer_num);
        sem_wait(&barber_chair);
    }
    else if(open_chairs == 0){
        printf("Customer %d realizes the barbershop is full and ain't nobody got time fo dat\n", customer_num);
        return;
    }
    else{
        num_customers++;
        printf("Customer %d is waiting for their haircut\n", customer_num);
        sem_wait(&empty);
        sem_wait(&barber_chair);
        sem_post(&empty);
    }
    sem_post(&begin_haircut);
    printf("Customer %d is receiving their haircut\n", customer_num);
    sleep(haircut_duration);
    printf("Customer %d's haircut is finished\n", customer_num);
    num_customers--;
}

// If shop is empty barber goes to sleep.
// He waits for a customer to wake him up and starts up the barber chair
// Waits for the signal to start the haircut given by the customer
void barber_thread(){
	while(1){
		sem_getvalue(&empty, &open_chairs);
        if(open_chairs == chairs && num_customers == 0){
            printf("The barber is sleeping\n");
            sem_wait(&barber);
            printf("The barber woke up\n");
        }
        sem_post(&barber_chair);
        sem_wait(&begin_haircut);
        printf("The barber is cutting a customers hair for %d seconds\n", haircut_duration);
	    sleep(haircut_duration);
	    printf("The barber finished the haircut\n\n");
	}
}

// setup all semaphores in a function so as to keep main cleaner.
void sem_setup(){
    sem_init(&empty, 0, chairs);
	sem_init(&barber_chair, 0, 0);
	sem_init(&barber, 0, 0);
	sem_init(&begin_haircut, 0, 0);
	sem_init(&customer_sem, 0, 1);
}

