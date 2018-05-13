#
# To compile, type "make" or make "all"
# To remove files, type "make clean"
#
OBJS = server.o pushServer.o request.o stems.o clientGet.o clientPost.o alarmClient.o
TARGET = server

CC = gcc
CFLAGS = -g -Wall

LIBS = -lpthread 

.SUFFIXES: .c .o 

all: server pushServer clientPost clientGet alarmClient dataGet.cgi dataPost.cgi alarm.cgi

server: server.o request.o stems.o
	$(CC) $(CFLAGS) -o server server.o request.o stems.o $(LIBS)

pushServer: pushServer.o request.o stems.o
	$(CC) $(CFLAGS) -o pushServer pushServer.o request.o stems.o $(LIBS)

clientGet: clientGet.o stems.o
	$(CC) $(CFLAGS) -o clientGet clientGet.o stems.o

clientPost: clientPost.o stems.o
	$(CC) $(CFLAGS) -o clientPost clientPost.o stems.o $(LIBS)

alarmClient: alarmClient.o stems.o
	$(CC) $(CFLAGS) -o alarmClient alarmClient.o stems.o $(LIBS)

dataGet.cgi: dataGet.c stems.o
	$(CC) $(CFLAGS) -o dataGet.cgi dataGet.c stems.o -lmysqlclient

dataPost.cgi: dataPost.c stems.o
	$(CC) $(CFLAGS) -o dataPost.cgi dataPost.c stems.o -lmysqlclient

alarm.cgi: alarm.c stems.o
	$(CC) $(CFLAGS) -o alarm.cgi alarm.c stems.o

.c.o:
	$(CC) $(CFLAGS) -o $@ -c $<

server.o: stems.h request.h
pushServer.o: stems.h request.h
clientGet.o: stems.h
clientPost.o: stems.h
alarmClient.o: stems.h	

clean:
	-rm -f $(OBJS) server pushServer clientPost clientGet alarmClient dataGet.cgi dataPost.cgi alarm.cgi
