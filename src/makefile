# temporary makefile to do intermittent testing

CC := gcc
CMPF := main.c 
OBJS := main.o 
FNAME := INJECTOR

INJECT-TARGET := target


default: target-bin
	@$(CC) -c -Wall $(CMPF)
	@$(CC) -Wall $(OBJS) -o $(FNAME)
	@rm -rf $(OBJS)
	./$(FNAME) $(INJECT-TARGET)
	

target-bin:
	@$(CC) -Wall test-bin.c -o $(INJECT-TARGET)

