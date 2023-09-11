/* event.c */

#include "mycal/event.h"

/**
 * Print event to stdout
 *
 * @param event 	event to print
 **/
void print_event(Event event) {

	/* check if valid event */

	if (event.valid) {
		puts("bad event");
		printf("%d\n", event.valid);
		return;
	}

	/* print event to stdout */

	// id
	printf("%s\n", event.id);
	
	// date
	char str[30];
	put_date(event.date, str);
	printf("%s\n", str);
	
	// time
	printf("%s\n", event.time);
	
	// duration 
	printf("%s\n", event.duration);
	
	// name
	printf("%s\n", event.name);
	
	// if avaible, print description or location
	if (event.description[0]) 
		printf("%s\n", event.description);
	if (event.location[0])
		printf("%s\n", event.location);
}

/**
 * Create an event from a string
 * Valid member of Event 0 if successful 
 * Valid member is set to different values for different errors
 *
 * @param buff 		buffer to make event from
 * return event 	new event
 **/
Event create_event(char *buff) {
	extern int id_count;


	/* initialize event */

	Event event = {0};

	/* parse json and set event values */

	cJSON *request = cJSON_Parse(buff);
	cJSON *item = NULL;
	if (request == NULL)
		event.valid = 1;

	// required 
	// Date
	item = cJSON_GetObjectItemCaseSensitive(request, "date");
	if (item == NULL) {
		event.valid = 2;
		return event;
	}
	Date date = get_date(item->valuestring);
	event.date = date;

	// Time
	item = cJSON_GetObjectItemCaseSensitive(request, "time");
	if (item == NULL || item->valuestring == NULL ) {
		event.valid = 3;
		return event;
	}
	strcpy(event.time, item->valuestring);

	
	// Duration
	item = cJSON_GetObjectItemCaseSensitive(request, "duration");
	if (item == NULL || item->valuestring == NULL ) {
		event.valid = 4;
		return event;
	}
	strcpy(event.duration, item->valuestring);


	// Name
	item = cJSON_GetObjectItemCaseSensitive(request, "name");
	if (item == NULL || item->valuestring == NULL ) {
		event.valid = 5;
		return event;
	}
	strcpy(event.name, item->valuestring);

		
	// optional
	// description
	item = cJSON_GetObjectItemCaseSensitive(request, "description");
	if (item != NULL) {	
		if (item->valuestring == NULL ) {
			event.valid = 6;
			return event;
		}
		strcpy(event.description, item->valuestring);
	}

	// location
	item = cJSON_GetObjectItemCaseSensitive(request, "location");
	if (item != NULL) {	
		if (item->valuestring == NULL ) {
			event.valid = 7;
			return event;
		}
		strcpy(event.location, item->valuestring);
	}

	// id
	item = cJSON_GetObjectItemCaseSensitive(request, "id");
	if (item == NULL) {
		sprintf(event.id, "%d", id_count);
		id_count++;
	} else {
		if (item->valuestring == NULL) {
			event.valid = 8;
			return event;
		}
		strcpy(event.id, item->valuestring);
	}

	// free cJSON		
	cJSON_Delete(request);

	return event;
}

/**
 * Convert event to a string 
 *
 * @param event 	event to convert to string
 * @return string 	string ptr to new string, must be freed
 **/
char* event_to_string(Event event) {
	// check if valid event
	if (event.valid) 
		return NULL;

	// create json
	cJSON *event_json = cJSON_CreateObject();
	char str[30];
	put_date(event.date, str);
	cJSON_AddStringToObject(event_json, "date", str);
	cJSON_AddStringToObject(event_json, "id", event.id);
	cJSON_AddStringToObject(event_json, "time", event.time);
	cJSON_AddStringToObject(event_json, "duration", event.duration);
	cJSON_AddStringToObject(event_json, "name", event.name);
	if (event.description[0])
		cJSON_AddStringToObject(event_json, "description", event.description);
	if (event.description[0])
		cJSON_AddStringToObject(event_json, "location", event.location);

	// free cJSON and set ptr - must be freed outside function
	char *string = cJSON_Print(event_json);
	cJSON_Delete(event_json);

	return string;
}

/**
 * Write event to file with given path
 *
 * @param path 		path to where to write file
 * @param event 	event to write
 **/
void write_event(char *path, Event event) {
	extern pthread_mutex_t Lock;
	pthread_mutex_lock(&Lock);
	FILE *file = fopen(path, "w+");
	char *string = event_to_string(event);
	fwrite(string, 1, (strlen(string) + 1), file);
	free(string);
	fclose(file);
	pthread_mutex_unlock(&Lock);
}

/**
 * Get event from file at path
 *
 * @param path 		path to event
 * @return event 	event at path
 **/
Event get_event(char *path) {
	extern pthread_mutex_t Lock;
	pthread_mutex_lock(&Lock);
	Event event = {0};
	char buffer[BUFSIZ];
	FILE *infile = fopen(path, "r");
	if (!infile) {
		event.valid = 1;	
	}
	
	if (!fread(buffer, 1, BUFSIZ, infile)) {
		event.valid = 1;		
	}
	event = create_event(buffer);
	fclose(infile);
	pthread_mutex_unlock(&Lock);
	return event;
}

/**
 * Updates a field of given event with value
 *
 * @param path 		path to event
 * @param field 	field to change
 * @param value 	value to change field to 
 * @return 			success
 **/
int update_event(char *path, char *field, char *value) {

	// make event from file and set new field
	Event event = {0};
	event = get_event(path);

	if (event.valid) 
		return 0;

	if (!strcmp(field, "date")) {
		remove(path);
		char *cal = strtok(path, "/");
		cal = strtok(NULL, "/");
		cal = strtok(NULL, "/");
		event.date = get_date(value);
		char *str = event_to_string(event);
		char id[BUFSIZ];
		add_event(str, id, cal);
		free(str);

		return 1;
		

	} else if (!strcmp(field, "time")) {
		strcpy(event.time, value);	
	} else if (!strcmp(field, "duration")) {
		strcpy(event.duration, value);
	} else if (!strcmp(field, "name")) {
		strcpy(event.name, value);
	} else if (!strcmp(field, "description")) {
	strcpy(event.description, value);
	} else if (!strcmp(field, "location")) {
		strcpy(event.location, value);
	}

	// write back new file
	write_event(path, event);

	return 1;

}

/**
 * Add a new event to calendar
 *
 * @param json 		json string to make into event
 * @param id 		id of event
 * @param cal 		calendar to add event to
 * @return 			success
 **/
int add_event(char *json, char *id, char *cal) {

	// create event
	Event event = create_event(json);


	// check if valid event
	if (event.valid) 
		return 1;	
	
	// structure:
	// data/calendar/year/month/day/id.json
	//
	// makes directories if not there, and if they are, mkdir does nothing
	// only one event per id, which is built from day and time, so only one event per time on day
	char path[40];
	char *year = event.date.year;
	char *month = event.date.month;
	char *day = event.date.day;
	strcpy(id,event.id);


	sprintf(path, "./data/%s", cal);
	mkdir(path, 0777);

	sprintf(path, "./data/%s/%s", cal, year);
	mkdir(path, 0777);

	sprintf(path, "./data/%s/%s/%s", cal, year, month);
	mkdir(path, 0777);

	sprintf(path, "./data/%s/%s/%s/%s", cal, year, month, day);
	mkdir(path, 0777);

	sprintf(path, "./data/%s/%s/%s/%s/%s.json", cal, year, month, day, id);

	write_event(path, event);	
	
	return 0;
}

/**
 * Find event based on id
 * Walks directory starting at root - recursive 
 *
 * @param root 		starting path
 * @param id		id/day to look for
 * @param dest 		array of paths to each event on day
 * @return n 		number of events found
 **/ 
int find_event(char *root, char *str, char dest[100][BUFSIZ]) {
	// Since event file name is build from day/id, use same fucntion to look for id or day
	// for day, if multiple events on same day, they are put into dest
	
	int n = 0;
	struct stat s;
	stat(root, &s);
	
	// walk directory
	if (S_ISDIR(s.st_mode)) {
		DIR *d = opendir(root);

		// loop through entries
		for (struct dirent *e = readdir(d); e; e = readdir(d)) {
			if (!strcmp(e->d_name, ".") || !strcmp(e->d_name, "..")) {
				continue;
			}

			// get path
			char path[BUFSIZ];
			sprintf(path, "%s/%s", root, e->d_name);
			stat(path, &s);

			// if dir, recurse
			// else, check if matching file
			if (S_ISDIR(s.st_mode)) {
				n += find_event(path, str, dest);
			} else {

				// search for day or id
				char b[BUFSIZ];
				strcpy(b, path);
				// .
				char *id = strtok(b, "/");
				// data
				id = strtok(NULL, "/");
				// cal
				id = strtok(NULL, "/");
				// year
				id = strtok(NULL, "/");
				// month
				id = strtok(NULL, "/");
				// day
				id = strtok(NULL, "/");
				// file
				id = strtok(NULL, ".");

				if (!strcmp(id, str)) {
					strcpy(dest[n], path);
					n++;
				}


				Event event = get_event(path);
				char date[BUFSIZ];
				put_date(event.date, date);

				if (!strcmp(date, str) && id[0] != '.') {
					strcpy(dest[n], path);
					n++;
				}


			}
		}
		closedir(d);
	}
	return n;
}

/**
 * Similar to find_event but with a range of days
 *
 * @param dest 			array of paths to events
 * @param start 		start day
 * @param stop 			stop day
 * @return num_events 	number of events found
 **/
int find_event_range(char dest[100][BUFSIZ], char *start, char *stop, char *cal) {
	// copy start day to curr_day
	char curr_day[30];
	strcpy(curr_day, start);

	int num_events = 0;
	int reachedEnd = 1;

	// while not passed stop day 
	while (reachedEnd) {
		// check last day

		if (!strcmp(curr_day, stop))
			reachedEnd = 0;

		// for given day, find events and add to dest
		int m;
		char d[100][BUFSIZ];
		char path[BUFSIZ];
		sprintf(path, "./data/%s", cal);
		if ((m = find_event(path, curr_day, d)) > 0) {
			for (int i = num_events; i < num_events + m; i++) {
				strcpy(dest[i], d[i-num_events]);
			}
			num_events += m;
		}
		
		// increment day, asssuming all months are 31 days for simplicty
		// mantains "0%d" otherise leading zero is lost
		Date date = get_date(curr_day);

		// add 1 to day
		int n = atoi(date.day) + 1;
		
		// if < 0, make sure to keep leading 0
		if (n < 10) {
			sprintf(date.day, "0%d", n);
		} else {
			sprintf(date.day, "%d", n);
		}

		// if > 31, reset day and increase month
		if (atoi(date.day) > 31) {
			strcpy(date.day, "00");
			
			// add 1 to month
			int n = atoi(date.month) + 1;

			// keep leading 0
			if (n < 10) {
				sprintf(date.month, "0%d", n);
			} else {
				sprintf(date.month, "%d", n);
			}

			// if > 12, increase year
			if (atoi(date.month) > 12) {
				strcpy(date.month, "00");
					
				// add 1 to year
				int n = atoi(date.year) + 1;

				// keep leading 0
				if (n < 10) {
					sprintf(date.year, "0%d", n);
				} else {
					sprintf(date.year, "%d", n);
				}

				// loop year
				if (atoi(date.year) > 99) {
					strcpy(date.year, "00");
				}
			}
		}

		put_date(date, curr_day);

	}

	return num_events;
}

