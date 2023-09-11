#ifndef SERVER_FUNC_H
#define SERVER_FUNC_H

#include "mycal/server_socket.h"
#include "mycal/cJSON.h"
#include "mycal/event.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <signal.h>
#include <pthread.h>

/* Constants */

#define SMALL_BUFF 		100
#define MAX_THREADS 	10

/* Structures */

typedef struct Arg Arg;

struct Arg {
	char buffer[BUFSIZ];
	int fd;
};

/* Functions */

void sig_handler(int);
int send_response(int, char *, char *, char *, char *, char *, char [SMALL_BUFF][BUFSIZ], int);
void *handle_request(void *);

#endif
