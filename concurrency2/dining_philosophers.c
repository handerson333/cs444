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

#include "rng.h"

struct philosopher
{
    char name[20];
    int fork_left;
    int fork_right;
    int seat;
};







//main
int main(int argc, char** argv)
{
    struct philosopher philosopher[4];

    strcpy(philosopher[0].name , "Albert Einstein");
    strcpy(philosopher[1].name , "John Locke");
    strcpy(philosopher[2].name , "Socrates");
    strcpy(philosopher[3].name , "Marcus Aurelius");
    strcpy(philosopher[4].name , "Plato");


    printf("1: %s\n2: %s\n3: %s\n4: %s\n5: %s\n\n", philosopher[0].name, philosopher[1].name, 
                                                    philosopher[2].name, philosopher[3].name, 
                                                    philosopher[4].name);
    
    return 0;
}








