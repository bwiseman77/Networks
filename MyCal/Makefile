CC= 		gcc

CFLAGS= 	-g -Wall -std=gnu99 -Iinclude -pthread 
TARGETS= 	mycalserver

# variables
HEADERS = $(wildcard inc/*.h)
SERVER_OBJS = src/server.o src/server_socket.o src/event.o src/date.o src/cJSON.o src/server_func.o

all: 		$(TARGETS)

src/%.o: src/%.c $(HEADERS)
	@echo Compiling $@...
	@$(CC) $(CFLAGS) -c -o $@ $<

mycalserver: $(SERVER_OBJS) $(HEADERS)
	@echo Compiling $@...
	@$(CC) $(CFLAGS) -o $@ $^

clean:
	@echo "cleaning up"
	@rm -fr data $(SERVER_OBJS) $(TARGETS)
