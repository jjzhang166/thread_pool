.PHONY:clean all
CC=gcc
CFLAGS=-Wall -g
ALL=main
all:$(ALL)
OBJS=thread_pool.o main.o condition.o
.c.o:
	$(CC) $(CFLAGES) -c $<
main:$(OBJS)
	$(CC) $(CFLAGES) $^ -o $@ -lpthread -lrt

clean:
	rm -f *.o 
