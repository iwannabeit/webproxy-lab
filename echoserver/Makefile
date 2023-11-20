
CC = gcc
CFLAGS = -g -Wall
LDFLAGS = -lpthread

all: echoclient echoserver

echoclient: echoclient.c csapp.o
	$(CC) $(CFLAGS) -o echoclient echoclient.c csapp.o $(LDFLAGS)

echoserver: echoserver.c echo.c csapp.o $(LDFLAGS)
	$(CC) $(CFLAGS) -o echoserver echoserver.c echo.c csapp.o $(LDFLAGS)

csapp.o: csapp.c csapp.h
	$(CC) $(CFLAGS) -c csapp.c

clean:
	rm -f echoclient echoserver echoclient.o echoserver.o csapp.o echo.o