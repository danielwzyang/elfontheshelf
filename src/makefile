# temporary makefile to do intermittent testing

CC := gcc
CMPF := main.c 
OBJS := main.o 
FNAME := injector.pro
INJECT-TARGET := target.pro


# do not merge this just keep it on owen branch so i can compile on mac
UNAME := $(shell uname)
ifeq ($(UNAME), Darwin)
	BREW_LIBELF := $(shell brew --prefix libelf 2>/dev/null)
	EXTRA_CFLAGS  := -I$(BREW_LIBELF)/include/libelf -I$(BREW_LIBELF)/include
	EXTRA_LDFLAGS := -L$(BREW_LIBELF)/lib -lelf
else
	EXTRA_CFLAGS  :=
	EXTRA_LDFLAGS :=
endif

default: target-bin
	@$(CC) -c -Wall $(CMPF)
	@$(CC) -Wall $(OBJS) -o $(FNAME)
	@rm -rf $(OBJS)
	@./$(FNAME) $(INJECT-TARGET)

target-bin:
	@$(CC) -Wall test-bin.c -no-pie -o $(INJECT-TARGET)

clean: 
	@rm -rf $(OBJS) $(INJECT-TARGET) $(FNAME)

