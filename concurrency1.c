#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include "mt19937ar.h"
const int PRODUCER_MAX_WAIT = 7;
const int PRODUCER_MIN_WAIT = 3;
const int CONSUMER_MIN_WAIT = 2;
const int CONSUMER_MAX_WAIT = 9;
#define BUFFER_SIZE  32
int RDRAND_OR_MERSENNE;


void check_rng_type();
int random_number_generator(int floor, int ceiling);


typedef struct
{
    short number;
    short wait;
} DATA;

struct DataContainer
{
    short consumerIdx;
    short producerIdx;
    DATA items[BUFFER_SIZE];
    pthread_mutex_t shareLock;
    pthread_cond_t consumerCond;
    pthread_cond_t producerCond;
};
struct DataContainer DC;

void printItem(DATA *myItem);

void *consumerFunc(void *foo);
void *producerFunc(void *foo);



int main(int argc, char** argv)
{
	if(argc > 3 )
	{
		perror("Usage: concurrency1 [# of consumer threads] [# of producer threads]");
		exit(1);
	}
	else if (argc < 3){
		perror("Usage: concurrency1 [# of consumer threads] [# of producer threads]");
		exit(1);

	}
	int consumer_threads = atoi(argv[1]);
	int producer_threads = atoi(argv[2]);
	int total_threads = consumer_threads + producer_threads;

	check_rng_type();

	DC.consumerIdx = 0;	
    DC.producerIdx = 0;

    pthread_t threads[total_threads];
    
    for(int i = 0; i < consumer_threads; i++){

        pthread_create(&threads[i], NULL, consumerFunc, NULL);
    }
    for(int i = 0; i < producer_threads; i++){

        pthread_create(&threads[i+1], NULL, producerFunc, NULL);
    }


    for(int i = 0; i < (total_threads); i++)
    {
        pthread_join(threads[i], NULL);
    }

    return 0;
}


void *producerFunc(void *foo)
{
    while(1)
    {

        pthread_mutex_lock(&DC.shareLock);
        
        DATA newItem;

        newItem.number = random_number_generator(1,100);
        newItem.wait = random_number_generator(CONSUMER_MIN_WAIT, CONSUMER_MAX_WAIT);
        int producer_wait = random_number_generator(PRODUCER_MIN_WAIT, PRODUCER_MAX_WAIT);
        printf("Producer waiting %d seconds.\n", producer_wait);
        sleep(producer_wait);
        printf("Producing Item:\n");
        printItem(&newItem);


        if(DC.producerIdx == BUFFER_SIZE)
        {
            printf("AT MAX size\n");
            pthread_cond_signal(&(DC.consumerCond));
            pthread_cond_wait(&(DC.producerCond), &DC.shareLock);
        }
        DC.items[DC.producerIdx] = newItem;
        DC.producerIdx++;

        pthread_cond_signal(&(DC.consumerCond));
        pthread_cond_wait(&(DC.producerCond), &DC.shareLock);
        if(DC.producerIdx >= BUFFER_SIZE)
        {
            printf("AT MAX size\n");
            DC.producerIdx = 0;
        }
        pthread_mutex_unlock(&DC.shareLock);
    }

}

void *consumerFunc(void *foo)
{
    while(1)
    {
        pthread_mutex_lock(&DC.shareLock);
        DATA consumeItem;
        if(DC.consumerIdx >= BUFFER_SIZE)
        {
            DC.consumerIdx = 0;
        }

        pthread_cond_signal(&(DC.producerCond));
        pthread_cond_wait(&(DC.consumerCond), &DC.shareLock);
        
        if(DC.producerIdx == 0)
        {
            printf("AT MAX size\n");
            pthread_cond_wait(&(DC.consumerCond), &DC.shareLock);
        }
        consumeItem = DC.items[DC.consumerIdx];
        printf("Consuming Item data: %d\nConsumer is waiting %d seconds...\n\n", consumeItem.number, consumeItem.wait);

        sleep(consumeItem.wait);
        
        DC.consumerIdx++;
        pthread_mutex_unlock(&DC.shareLock);
    }
}





void printItem(DATA *myItem)
{
    static int i = 0;
    printf("%d:",i);
    i++;

    printf("\tItem number: %d\n", myItem->number);
    printf("\tItem wait: %d\n\n", myItem->wait);
}






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

	if ( num < 0){
		num *= -1;		
	}
	num = num % (ceiling - floor);
	
	if (num < floor){
		num = ceiling - num;		
	}

	return num;	

}
