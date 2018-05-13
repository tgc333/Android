#
# To compile, type "make" or make "all"
# To remove files, type "make clean"
#
OBJS = dht11.o stems.o
TARGET = dht11
 
CC = gcc
CFLAGS = -g -Wall
 
LIBS = -lpthread 
 
.SUFFIXES: .c .o 
 
all: dht11
 
dht11: dht11.o stems.o
	$(CC) $(CFLAGS) -o dht11 dht11.o stems.o $(LIBS) -lwiringPi
 
 
.c.o:
	$(CC) $(CFLAGS) -o $@ -c $<
 
dht11.o: stems.h
 
clean:
	-rm -f $(OBJS) dht11