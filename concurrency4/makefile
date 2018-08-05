CC:=gcc
CFLAGS:= -w -pthread -std=c99

all: barbershop smokers

barbershop:
		$(CC) barbershop.c $(CFLAGS) -o barbershop

smokers:
	$(CC) smokers.c $(CFLAGS) -o smokers

clean:
	rm smokers barbershop