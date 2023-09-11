#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <errno.h>
#include <string.h>
#include <sys/wait.h>

#define BACKLOG 10

int socket_listen(const char *port);
int socket_accept(int server_fd);

