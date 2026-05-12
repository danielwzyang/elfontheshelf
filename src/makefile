# temporary makefile to do intermittent testing

CC := gcc
CMPF = main.c 
OBJS = main.o 
FNAME = INJECTOR

default: 
	$(CC) -c -Wall $(CMPF)
	$(CC) -Wall $(OBJS) -o $(FNAME)
	rm -rf $(OBJS)
	
