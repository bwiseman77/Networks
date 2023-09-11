#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>

void *get_in_addr(struct sockaddr *sa);
int socket_dial(const char* host, const char* port);
