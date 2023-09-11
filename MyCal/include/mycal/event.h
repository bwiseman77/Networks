/* event.h: event object */

#ifndef EVENT_H
#define EVENT_H

#include "mycal/cJSON.h"
#include "mycal/date.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pthread.h>

/* Structures */

typedef struct Event Event;

struct Event {
	int valid;
	Date date;
	char time[30];
	char duration[30];
	char name[256];
	char description[2048];
	char location[1024];
	char id[30]; 
};

/* Functions */

void 		print_event(Event);
Event 		create_event(char *);
char * 		event_to_string(Event);

void 		write_event(char *, Event);
int 		update_event(char *, char *, char *);
int 		add_event(char *, char *, char *);
Event 		get_event(char *);
int 		find_event(char *, char *, char [100][BUFSIZ]);
int 		find_event_range(char [100][BUFSIZ], char *, char *, char *);

#endif


