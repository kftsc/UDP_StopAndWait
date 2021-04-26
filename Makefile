CC=gcc
CFLAGS=-Wall

all: Server Client cleanOutput

Server: Server.o -lm HandleError.o 

Client: Client.o HandleError.o 

HandleError.o: HandleError.c
	$(CC) -c HandleError.c

Server.o: Server.c
	$(CC) -c Server.c

Client.o: Client.c
	$(CC) -c Client.c

clean: 
	rm -f HandleError.o Server.o Client.o Server Client out.txt

cleanOutput:
	rm -f out.txt