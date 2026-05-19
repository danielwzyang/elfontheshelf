# temporary makefile to do intermittent testing

CC := gcc
CMPF := main.c helpers.c
OBJS := main.o helpers.o
FNAME := injector.pro
INJECT-TARGET := target.pro


default: target-bin
	@$(CC) -c -Wall $(CMPF)
	@$(CC) -Wall $(OBJS) -o $(FNAME)
	@rm -rf $(OBJS)
	@./$(FNAME) $(INJECT-TARGET) ../exec.bin

target-bin:
	@$(CC) -Wall test-bin.c -no-pie -o $(INJECT-TARGET)

clean: 
	@rm -rf $(OBJS) $(INJECT-TARGET) $(FNAME)

