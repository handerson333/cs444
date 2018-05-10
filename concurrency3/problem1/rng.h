
#include "mt19937ar.h"
#include <stdlib.h>
#include <stdio.h>

//global
int RDRAND_OR_MERSENNE;



void check_rng_type();
int random_number_generator(int floor, int ceiling);
int rng(int min, int max);

int rng(int min, int max)
{
    check_rng_type();
    return random_number_generator( min, max );

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

    return;

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