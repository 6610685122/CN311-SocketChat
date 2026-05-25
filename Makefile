CC = gcc
CFLAGS = -Wall -pthread -I./include

all: server client

server: src/server.c src/protocol.c
	$(CC) $(CFLAGS) -o server src/server.c src/protocol.c

client: src/client.c src/protocol.c
	$(CC) $(CFLAGS) -o client src/client.c src/protocol.c

clean:
	rm -f server client