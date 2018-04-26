
#include "mt19937ar.h"
#include <stdlib.h>
#include <stdio.h>

//global
int RDRAND_OR_MERSENNE;
const int EATING_MIN_TIME = 2;
const int EATING_MAX_TIME = 9;
const int THINKING_MIN_TIME = 1;
const int THINKING_MAX_TIME = 20;

int rng(int thinking)
{
    check_rng_type();
    if (thinking){
        return random_number_generator( THINKING_MIN_TIME, THINKING_MAX_TIME );
    }
    else
    {
        return random_number_generator( EATING_MIN_TIME, EATING_MAX_TIME );
    }
}


//get rng type
int check_rng_type()
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