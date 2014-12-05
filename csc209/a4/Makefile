PORT=50473
CFLAGS = -DPORT=\$(PORT) -Wall -g

all: calserver

calserver: calserver.o lists.o
		gcc $(CFLAGS) -o calserver calserver.o lists.o

calserver.o: calserver.c lists.h
		gcc $(CFLAGS) -c calserver.c

lists.o: lists.c lists.h
		gcc $(CFLAGS) -c lists.c

clean:
		rm calserver *.o