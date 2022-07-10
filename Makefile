CC=gcc
CFLAGS=-std=c99 -Wall -Wextra -g
LOAD=load_balancer
SERVER=server
HASH= Hashtable
LIST=LinkedList
UTIL=utils

.PHONY: build clean pack

build: tema2

tema2: main.o $(LOAD).o $(SERVER).o $(HASH).o $(LIST).o $(UTIL).o
	$(CC) $^ -o $@

main.o: main.c
	$(CC) $(CFLAGS) $^ -c

$(SERVER).o: $(SERVER).c $(SERVER).h
	$(CC) $(CFLAGS) $^ -c

$(LOAD).o: $(LOAD).c $(LOAD).h
	$(CC) $(CFLAGS) $^ -c

$(HASH).o: $(HASH).c $(HASH).h
	$(CC) $(CFLAGS) $^ -c

$(LIST).o: $(LIST).c $(LIST).h
	$(CC) $(CFLAGS) $^ -c

$(UTIL).o: $(UTIL).c $(UTIL).h
	$(CC) $(CFLAGS) $^ -c

pack:
	zip -FSr 315CA_RaresButilca_Tema2.zip README Makefile *.c *.h

clean:
	rm -f *.o tema2 *.h.gch
