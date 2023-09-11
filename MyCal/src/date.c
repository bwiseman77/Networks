/* date.c */
#include "mycal/date.h"

/**
 * Prints Date to stdout
 *
 * @param date 		date to print
 **/
void print_date(Date date) {
	printf("%s\n", date.month);
	printf("%s\n", date.day);
	printf("%s\n", date.year);
}

/**
 * Creates date from string
 *
 * @param d 		d to make into date
 * @return date 	date struct
 **/
Date get_date(char *d) {
	Date date;
	date.month[0] = d[0];
	date.month[1] = d[1];
	date.month[2] = '\0';
	date.day[0] = d[2];
	date.day[1] = d[3];
	date.day[2] = '\0';
	date.year[0] = d[4];
	date.year[1] = d[5];
	date.year[2] = '\0';

	return date;
}

/**
 * Converts Date struct to single string 
 *
 * @param date 		date to convert to string
 * @param str 		string to put date in
 **/
void put_date(Date date, char *str) {
	sprintf(str, "%s%s%s", date.month, date.day, date.year);	
}

