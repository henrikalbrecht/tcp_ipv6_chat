SERVER = src/server.c
CLIENT = src/client.c
CFLAGS = -Wall
CC = gcc

all: server client

server:
	mkdir -p build
	$(CC) $(CFLAGS) -o build/server $(SERVER) 

client:
	$(CC) $(CFLAGS) -o build/client $(CLIENT)
		
clean:
		rm -rf build
