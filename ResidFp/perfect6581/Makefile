OBJS=perfect6581.o netlist_sim.o
OBJS+=test.o
#CFLAGS=-Werror -Wall -Wextra -pedantic -O3
#CC=clang
CFLAGS+=-std=c99

all: test

test: $(OBJS)
	$(CC) -o test $(OBJS)

clean:
	rm -f $(OBJS) test

