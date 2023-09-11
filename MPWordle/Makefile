CC= 		gcc
CFLAGS= 	-g -Wall -std=gnu99 -Iinclude -pthread 
TARGETS= 	mpwordleserver mpwordle

# variables
SERVER_OBJS = src/server.o src/server_socket.o src/cJSON.o src/wordle.o src/wordle_server.o
CLIENT_OBJS = src/client.o src/client_socket.o src/cJSON.o src/wordle.o

all: 		$(TARGETS)

src/%.o: src/%.c
	@echo Compiling $@...
	@$(CC) $(CFLAGS) -c -o $@ $^

mpwordleserver: $(SERVER_OBJS)
	@echo Compiling $@...
	@$(CC) $(CFLAGS) -o $@ $^

mpwordle: $(CLIENT_OBJS)
	@echo Compiling $@...
	@$(CC) $(CFLAGS) -o $@ $^

clean:
	@echo "cleaning up"
	@rm -fr $(SERVER_OBJS) $(CLIENT_OBJS) $(TARGETS)
