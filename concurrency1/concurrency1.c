/*
Robert Hayden Anderson
CS444 Operating Systems 2
Prof: Kevin McGrath
Concurrency assignment 1


sources:
http://pages.cs.wisc.edu/~remzi/Classes/537/Fall2008/Notes/threads-semaphores.txt
https://stackoverflow.com/questions/14888027/mutex-lock-threads
http://man7.org/linux/man-pages/man3/pthread_create.3.html
http://man7.org/linux/man-pages/man3/pthread_join.3.html
https://github.com/tnoelcke/CS444-S2018/blob/master/C1/concurrency.c
https://github.com/zainkai/CS444/blob/master/Concurrency1/concurrency1.c
http://greenteapress.com/semaphores/LittleBookOfSemaphores.pdf
https://oregonstate.instructure.com/courses/1671300/assignments/7246802#submit
*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include "mt19937ar.h"

//constant globals
const int PRODUCER_MAX_WAIT = 7;
const int PRODUCER_MIN_WAIT = 3;
const int CONSUMER_MIN_WAIT = 2;
const int CONSUMER_MAX_WAIT = 9;
#define BUFFER_SIZE  32

//global
int RDRAND_OR_MERSENNE;



//structs

//base struct holding the two numbers
typedef struct
{
    int number;
    int wait;
} DATA;

//struct to hold the data with indexes and threads
struct DATA_container
{
    
    DATA items[BUFFER_SIZE];
    int consumer_index;
    int producer_index;
    pthread_mutex_t shared_lock;
    pthread_cond_t consumer_condition;
    pthread_cond_t producer_condition;
};
struct DATA_container container;


//function declarations
void check_rng_type();
int random_number_generator(int floor, int ceiling);
void print_item(DATA *item);
void *consume(void *foo);
void *produce_item(void *foo);



//main
int main(int argc, char** argv)
{
    //make sure there are 2 arguments for # of threads
	if(argc > 3 )
	{
		perror("Usage: concurrency1 [# of consumer threads] [# of producer threads]");
		exit(1);
	}
	else if (argc < 3){
		perror("Usage: concurrency1 [# of consumer threads] [# of producer threads]");
		exit(1);

	}
    //get threads from cla's
	int consumer_threads = atoi(argv[1]);
	int producer_threads = atoi(argv[2]);
	int total_threads = consumer_threads + producer_threads;

    //check system type for RDRAND or MT
	check_rng_type();

    //set indexes at 0
	container.consumer_index = 0;	
    container.producer_index = 0;

    
    pthread_t threads[total_threads];
    //counter
    int i =0;

    //create the threads for both consumer and producer until they are both down to 0
    do
    {
        if(consumer_threads != 0)
        {
            pthread_create(&threads[i], NULL, consume, NULL);
            i++;
            consumer_threads--;
        }
        if(producer_threads != 0)
        {
            pthread_create(&threads[i], NULL, produce_item, NULL);
            i++;
            producer_threads--;
        }

    }while ( consumer_threads != 0 && producer_threads != 0 );


    //join all of the threads
    for(int i = 0; i < (total_threads); i++)
    {
        pthread_join(threads[i], NULL);
    }

    return 0;
}



//item producer function for the queue
void *produce_item(void *foo)
{
    while(1)
    {
        //lock the thread
        pthread_mutex_lock(&container.shared_lock);
        
        //create data object
        DATA item;

        //get a random number for first value
        item.number = random_number_generator(0,100);
        //find how long consumer will have to wait for consumption time
        item.wait = random_number_generator(CONSUMER_MIN_WAIT, CONSUMER_MAX_WAIT);

        //get a random number for producing time
        int producer_wait = random_number_generator(PRODUCER_MIN_WAIT, PRODUCER_MAX_WAIT);
        printf("Producer is waiting %d seconds to produce an item.\n", producer_wait);
        
        //wait 3-7 seconds
        sleep(producer_wait);
        print_item(&item);
        

        //make sure we didnt get to max buffer size. If so, change conditions.
        if(container.producer_index == BUFFER_SIZE)
        {
            printf("Buffer is filled\n");
            pthread_cond_signal(&(container.consumer_condition));
            pthread_cond_wait(&(container.producer_condition), &container.shared_lock);
        }
        container.items[container.producer_index] = item;
        container.producer_index += 1;

        pthread_cond_signal(&(container.consumer_condition));
        pthread_cond_wait(&(container.producer_condition), &container.shared_lock);
        if(container.producer_index >= BUFFER_SIZE)
        {
            printf("Buffer is filled\n");
            container.producer_index = 0;
        }
        pthread_mutex_unlock(&container.shared_lock);
    }

}


//consume item function
void *consume(void *foo)
{
    while(1)
    {
        //lock thread
        pthread_mutex_lock(&container.shared_lock);
        DATA consumeItem;
        if(container.consumer_index >= BUFFER_SIZE)
        {
            container.consumer_index = 0;
        }
        //signal producer
        pthread_cond_signal(&(container.producer_condition));
        //wait for producer to give thread
        pthread_cond_wait(&(container.consumer_condition), &container.shared_lock);
        

        //make sure there's an item to consume. If so, change conditions.
        if(container.producer_index == 0)
        {
            printf("Buffer is empty\n");
            pthread_cond_wait(&(container.consumer_condition), &container.shared_lock);
        }
        //Actually consume items
        consumeItem = container.items[container.consumer_index];
        printf("Consuming Item data: %d\nConsumer is waiting %d seconds...\n\n", consumeItem.number, consumeItem.wait);

        //sleep amount of time for second variable
        sleep(consumeItem.wait);
        //make consumer go to next position in the buffer
        container.consumer_index += 1;

        //unlock thread
        pthread_mutex_unlock(&container.shared_lock);
    }
}




//print item for producer
void print_item(DATA *item)
{
    static int i = 0;
    
    printf("Producing Item %d:\n", i);
    printf("Item number: %d\n", item->number);
    printf("Item wait: %d\n\n", item->wait);

    i++;
}





//get rng type
void check_rng_type()
{
	unsigned int eax;
	unsigned int ebx;
	unsigned int ecx;
	unsigned int edx;
	
	eax = 0x01;

	__asm__ __volatile__(
	                     "cpuid;"
	                     : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx)
	                     : "a"(eax)
	                     );
	
	if(ecx & 0x40000000){
		//use rdrand
		RDRAND_OR_MERSENNE = 1;
	}
	else{
		//use mt19937
		RDRAND_OR_MERSENNE = 0;
	}



}

//get random number function
int random_number_generator(int floor, int ceiling)
{

	int num;
	if (RDRAND_OR_MERSENNE == 1)
	{
		//RDRAND
		__asm__ __volatile__("rdrand %0":"=r"(num));
		//printf("num: %d\n\n", num);
						
	}
	else
	{
		//Mersenne Twister
		num =  (int)genrand_int32();
		//printf("num: %d\n\n", num);
	}

    //make sure number is positive. If not, make it so.
	if ( num < 0){
		num *= -1;		
	}
    //mod the number by the difference in ceiling and floor
	num = num % (ceiling - floor);
	
    //wrap back arouind if number goes less than floor
	if (num < floor){
		num = ceiling - num;		
	}

	return num;	

}
