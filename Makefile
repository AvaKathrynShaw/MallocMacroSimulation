COMPILER = gcc
CFLAGS = -Wall -g
CFLAGS2 = -g

all: mymalloc 
.PHONY: all
.PHONY: clean

mymalloc.o: mymalloc.c mymalloc.h
	$(COMPILER) $(CFLAGS) -c mymalloc.c
	
mymalloc: mymalloc.o
	$(COMPILER) $(CFLAGS) -o mymalloc mymalloc.o


clean:
	rm -f *.o mymalloc 
