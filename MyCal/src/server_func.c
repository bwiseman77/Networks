/* server_func.c */

#include "mycal/server_func.h"

/* Functions */

/**
 * Signal handler to quit peacefully with Control-C
 *
 * @param signo 		signal number
 **/
void sig_handler(int signo) {
	extern int thread_count;
	extern pthread_t threads[MAX_THREADS];

	if (signo == SIGINT) {		
		for (int i = 0; i < thread_count; i++) {
			pthread_join(threads[i], NULL);
		}

		extern int id_count;
		FILE *cfile = fopen("./data/count.txt", "w+");
		fprintf(cfile, "%d", id_count);
		fclose(cfile);
		exit(EXIT_SUCCESS);	
	}
}

/**
 * Sends a reponse from server to client over network
 * Response is a JSON containing information about the action 
 * 
 * @param fd 		file descriptor to socket
 * @param cmd 		command field
 * @param cal 		calendar field
 * @param id 		identifier field
 * @param success   success field
 * @param msg       message field
 * @param dest      array of events to add
 * @param n    		number of events to add
 * @return 			success or failure
 **/		
int send_response(int fd, char *cmd, char *cal, char *id, char *success, char *msg, char dest[100][BUFSIZ], int n) {
	char buffer[BUFSIZ];
	
	// create reponse json
	cJSON *response = cJSON_CreateObject();

	cJSON_AddStringToObject(response, "Command", cmd);
	cJSON_AddStringToObject(response, "Calendar", cal);
	cJSON_AddStringToObject(response, "id", id);
	cJSON_AddStringToObject(response, "success", success);
	cJSON_AddStringToObject(response, "message", msg);
	cJSON *data = cJSON_CreateObject();

	char c[SMALL_BUFF];
	// create data json
	// loop through events and add them to data
	for (int i = 0; i < n; i++) {

		sprintf(c, "%d", i);

		Event event = get_event(dest[i]);

		cJSON *event_json = cJSON_CreateObject();
		char str[SMALL_BUFF];
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

		cJSON_AddItemToObject(data, c, event_json);

	}
	cJSON_AddItemToObject(response, "data", data);

	char *str = cJSON_Print(response);
	cJSON_Delete(response);

	strcpy(buffer, str);
	buffer[strlen(str) + 1] = '\0';
	send(fd, buffer, (strlen(buffer)+1), 0);
		
	printf("\nresponse :\n%s\n", buffer);
	fflush(stdout);
	free(str);

	return 1;
}

/**
 * Thread function
 * Handles the requests of connection clients
 *
 * @param args 		Arg struct that contains string and fd
 **/
void *handle_request(void *args) {
	// set up
	//char buffer[BUFSIZ];	
	char idb[BUFSIZ];
	char *id;
	char dest[SMALL_BUFF][BUFSIZ];
	int n;

	Arg *arg = (Arg *)args;
	
	int fd = arg->fd;
	char *cal = strtok(arg->buffer, " ");
	char *cmd = strtok(NULL, " ");

	char path[BUFSIZ];
	char smsg[BUFSIZ];
	sprintf(path, "./data/%s", cal);
	
	/* add command */
	if (strstr(cmd, "add")) {
		char *json = strtok(NULL, "\0");
		if (add_event(json, idb, cal)) {
			send_response(fd, "add", cal, idb, "false", "invalid event", NULL, 0);
		} else {
			sprintf(smsg, "added event in %s with id %s", cal, idb);
			send_response(fd, "add", cal, idb, "true", smsg, NULL, 0);
		}

	/* getrange command */
	} else if (strstr(cmd, "getrange")) {
		char *start = strtok(NULL, " ");
		char *stop = strtok(NULL, "\n");
		if ((n = find_event_range(dest, start, stop, cal)) > 0) {
			printf("%d events found\n", n);
			sprintf(smsg, "%d event(s) found from %s to %s", n, start, stop);
			send_response(fd, "getrange", cal, " ", "true", smsg,  dest, n);
		} else {
			send_response(fd, "getrange", cal, " ", "true", "No events",  dest, n);
		}

	/* remove command */
	} else if (strstr(cmd, "remove")) {
		id = strtok(NULL, "\n");

		if (find_event(path, id, dest)) {
			remove(dest[0]);
			sprintf(smsg, "removed event %s", id);		
			send_response(fd, "remove", cal, id, "true", smsg, dest, 0);

		} else {
			send_response(fd, "remove", cal, id, "false", "could not find event", dest, 0);
		}

	/* update command */
	} else if (strstr(cmd, "update")) {
		id = strtok(NULL, " ");
		char *field = strtok(NULL, " ");
		char *value = strtok(NULL, "\n");
		if (find_event(path, id, dest)) {
			update_event(dest[0], field, value);
			sprintf(smsg, "updated event %s's field %s to %s", id, field, value);			
			send_response(fd, "update", cal, id, "true", smsg, dest, 0);
		} else {
			send_response(fd, "update", cal, id, "false", "could not find event", dest, 0);
		}

	/* get command */
	} else if (strstr(cmd, "get")) {
		id = strtok(NULL, "\n");

		if ( (n = find_event(path, id, dest)) > 0) {
			char data[SMALL_BUFF];
			sprintf(data, "%d", n);

			sprintf(smsg, "%d event(s) found on event %s", n, id);
			send_response(fd, "get", cal, id, "true", smsg, dest, n);
			
		} else {
			send_response(fd, "get", cal, id, "false", "could not find event", dest, 0);
		}

	/* default */	
	} else {
		send_response(fd, "unknown", cal, " ", "false", "invalid command", NULL, 0);
	}


	return 0;	
}

