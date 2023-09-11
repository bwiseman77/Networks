/* date.h: date structure */

#ifndef DATE_H
#define DATE_H

#include <string.h>
#include <stdio.h>

/* Structures */

typedef struct Date Date;

struct Date {
	char month[10];
	char day[10];
	char year[10];
};

/* Functions */

void 	print_date(Date);
Date 	get_date(char *);
void 	put_date(Date, char *);

#endif
