#include "wordle/wordle_server.h"
#include <ctype.h>

/* globals */

extern int Players;
extern Client_info Clients[];
extern Game_info Game;
extern pthread_mutex_t Lock;

/* functions */

int get_words(char words[MAX_WORDS][MAX_WORD_LENGTH], char *path) {

	FILE *infile = fopen(path, "r");
	if (!infile) return -1;
	int ind = 0;
	while (fgets(words[ind], MAX_WORD_LENGTH, infile)) {
		// trim newline;
		words[ind][strlen(words[ind])-1] = '\0';
		ind++;	

	}

	return ind;
}


cJSON *get_players(Type type) {
	cJSON *array = cJSON_CreateArray();


	for (int i = 0; i < Players; i++) {
		if (!Clients[i].inGame) continue;
		// build item {}
		cJSON *item = cJSON_CreateObject();
		if (type == START_GAME) {
			cJSON_AddStringToObject(item, "Name", Clients[i].name);
			cJSON_AddNumberToObject(item, "Number", Clients[i].number);

		} else if (type == START_ROUND) {
			cJSON_AddStringToObject(item, "Name", Clients[i].name);
			cJSON_AddNumberToObject(item, "Number", Clients[i].number);
			cJSON_AddNumberToObject(item, "Score", Clients[i].Tscore);
		
		} else if (type == GUESS_RESULT) {
			cJSON_AddStringToObject(item, "Name", Clients[i].name);
			cJSON_AddNumberToObject(item, "Number", Clients[i].number);
			
			if (Game.winner) 
				cJSON_AddStringToObject(item, "Correct", "Yes");
			else 
				cJSON_AddStringToObject(item, "Correct", "No");
			cJSON_AddNumberToObject(item, "ReceiptTime", Clients[i].time);
			cJSON_AddStringToObject(item, "Result", Clients[i].byg);

		} else if (type == END_ROUND) {
			cJSON_AddStringToObject(item, "Name", Clients[i].name);
			cJSON_AddNumberToObject(item, "Number", Clients[i].number);
			cJSON_AddNumberToObject(item, "ScoreEarned", Clients[i].Rscore);
			if (Clients[i].correct) 
				cJSON_AddStringToObject(item, "Winner", "Yes");
			else 
				cJSON_AddStringToObject(item, "Winner", "No");

		} else if (type == END_GAME) {
			cJSON_AddStringToObject(item, "Name", Clients[i].name);
			cJSON_AddNumberToObject(item, "Number", Clients[i].number);
			cJSON_AddNumberToObject(item, "Score", Clients[i].Tscore);


		} else {
			return NULL;
		}

		// add to list []
		cJSON_AddItemToArray(array, item);
	
	}

	return array;
}

void send_message(char *str, char *name, int fd, bool dbg, bool inGame) {
	if (!inGame) return;
	
	pthread_mutex_lock(&Lock);
	//printf("%s|\n", str);
	
	char *s;
	Message *msg = message_from_command(str, name);
	// printf("fd: %d\n", fd);
	//
	msg->players =get_players(msg->type);

	//printf("type : %d\n", msg->type); 
	s = message_to_json(msg, msg->type);
	if (dbg) printf("reponse: %ld %s\n", strlen(s) + 1, s);
	send(fd, s, BUFSIZ, 0);
	// printf("%d\n", b);
	free(s);
	//if (msg->players) cJSON_Delete(msg->players);
	free(msg);

	pthread_mutex_unlock(&Lock);
}
