#include "mycal/client_socket.h"
#include "mycal/cJSON.h"

int main(int argc, char *argv[]) {
	char buffer[BUFSIZ];
	char request[BUFSIZ];
	char port[BUFSIZ];
	char host[BUFSIZ];

	// set up config
	FILE *fp = fopen(".mycal", "r");
	
	if (!fp) {
		puts("invalid config file");
		fclose(fp);
		exit(EXIT_FAILURE);
	}

	if (!fread(buffer, 1, BUFSIZ, fp)) {
		puts("bad read");
		fclose(fp);
		exit(EXIT_FAILURE);
	}

	fclose(fp);

	cJSON *config = cJSON_Parse(buffer);
	cJSON *item = cJSON_GetObjectItemCaseSensitive(config, "servername");
	if (item == NULL) {
		puts("invalid config");	
		cJSON_Delete(config);
		exit(EXIT_FAILURE);
	}

	strcpy(host, item->valuestring);

	
	item = cJSON_GetObjectItemCaseSensitive(config, "port");
	if (item == NULL) {
		puts("invalid config");	
		cJSON_Delete(config);
		exit(EXIT_FAILURE);
	}

	strcpy(port, item->valuestring);
	cJSON_Delete(config);



	// read args
	if (argc < 2) {
		puts("not enough input");
		exit(EXIT_FAILURE);
	}
	int argind = 1;
	char *cal = argv[argind++];
	char *cmd = argv[argind++];
	if (!strcmp(cmd, "input")) {
		if (argc != 4) {
			puts("bad input");
			exit(EXIT_FAILURE);
		}
		FILE *infile = fopen(argv[argind], "r");
		if (!infile) {
			fclose(infile);
			puts("bad file");
			exit(EXIT_FAILURE);
		}

		if(!fread(buffer, 1, BUFSIZ, infile)) {
			puts("bad read");
			fclose(infile);
			exit(EXIT_FAILURE);
		}

		cJSON *file_json = cJSON_Parse(buffer);
		cJSON *commands = cJSON_GetObjectItemCaseSensitive(file_json, "commands");
		cJSON *command;


		int size = cJSON_GetArraySize(commands);
		for (int i = 0; i < size; i++) {
			item = cJSON_GetArrayItem(commands, i);
			command = cJSON_GetObjectItemCaseSensitive(item, "cmd");
			char *c = cJSON_Print(command);
			char *string = cJSON_Print(item);
			sprintf(buffer, "%s %s %s\n", cal, c, string);
			printf("%s\n", buffer);
			
			int fd = socket_dial(host, port);	
			send(fd, buffer, BUFSIZ, 0);   

			recv(fd, buffer, 2*BUFSIZ, 0);
			printf("%s", buffer);
		}
		return 0;
	}

	if (!strcmp(cmd, "add")) {
		cJSON *event_json = cJSON_CreateObject();
		if ((argc-3) % 2) {
			puts("not enough args");
			exit(EXIT_FAILURE);
		}
		while(argind+1 < argc) {
			cJSON_AddStringToObject(event_json, argv[argind], argv[argind+1]);
			argind+=2;
		}

		char *string = cJSON_Print(event_json);
		cJSON_Delete(event_json);
		strcpy(buffer, string);
		free(string);
		
	} else if (!strcmp(cmd, "remove")) {
		if (argc != 4) {
			puts("bad input");
			exit(EXIT_FAILURE);
		}
		strcpy(buffer, argv[argind]);
	} else if (!strcmp(cmd, "update")) {
		if (argc != 6) {
			puts("bad input");
			exit(EXIT_FAILURE);
		}
		sprintf(buffer, "%s %s %s", argv[argind], argv[argind+1], argv[argind+2]);
	} else if (!strcmp(cmd, "get")) {
		if (argc != 4) {
			puts("bad input");
			exit(EXIT_FAILURE);
		}
		strcpy(buffer, argv[argind]);

	} else if (!strcmp(cmd, "getrange")) {
		if (argc != 5) {
			puts("bad input");
			exit(EXIT_FAILURE);
		}
		sprintf(buffer, "%s %s", argv[argind], argv[argind+1]);
		
	} else {
		puts("bad command");
		exit(EXIT_FAILURE);
	}

	sprintf(request, "%s %s %s", cal, cmd, buffer);

	int fd = socket_dial(host, port);
	printf("\nrequest: \n%s\n", request);
	send(fd, request, BUFSIZ, 0);   

	recv(fd, buffer, 2*BUFSIZ, 0);
	printf("\nresponse: \n%s\n", buffer);
	return 0;
}

