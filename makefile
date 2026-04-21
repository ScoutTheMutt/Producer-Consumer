CC=gcc
CFLAGS=-I.
DEPS = ./headers/LinkedList.h

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

ProducerConsumerProblem: ./files/ProducerConsumer.c ./files/LinkedList.c
	$(CC) -o ProducerConsumerProblem ./files/ProducerConsumer.c ./files/LinkedList.c