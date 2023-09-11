#include "wordle/wordle.h"
#include <ctype.h>
#define CLIENT "bwisemaSgood"

/* functions */

char *message_to_json(Message *msg, Type type) {
	cJSON *json = cJSON_CreateObject();
	cJSON *items = cJSON_CreateObject();
	switch (type) {
		case JOIN:	
			cJSON_AddStringToObject(items, "Name", msg->name);
			cJSON_AddStringToObject(items, "Client", msg->client);
			cJSON_AddItemToObject(json, "Data", items); 
			cJSON_AddStringToObject(json, "MessageType", "join");
			break;
		case CHAT:
			cJSON_AddStringToObject(items, "Name", msg->name);
			cJSON_AddStringToObject(items, "Text", msg->text);
			cJSON_AddItemToObject(json, "Data", items); 
			cJSON_AddStringToObject(json, "MessageType", "chat");
		    break;
		case JOIN_RESULT:	
			cJSON_AddStringToObject(items, "Name", msg->name);
			cJSON_AddStringToObject(items, "Result", msg->result);
			cJSON_AddItemToObject(json, "Data", items); 
			cJSON_AddStringToObject(json, "MessageType", "JoinResult");
			break;
		case START_INSTANCE:
			cJSON_AddStringToObject(items, "Server", msg->server);
			cJSON_AddStringToObject(items, "Port", msg->port);
			cJSON_AddNumberToObject(items, "Nonce", msg->nonce);
			cJSON_AddItemToObject(json, "Data", items); 
			cJSON_AddStringToObject(json, "MessageType", "StartInstance");
			break;
		case JOIN_INSTANCE:
			cJSON_AddStringToObject(items, "Name", msg->name);
			cJSON_AddNumberToObject(items, "Nonce", msg->nonce);
			cJSON_AddItemToObject(json, "Data", items); 
			cJSON_AddStringToObject(json, "MessageType", "JoinInstance");
			break;
		case JOIN_INSTANCE_RESULT:
			cJSON_AddStringToObject(items, "Name", msg->name);
			cJSON_AddStringToObject(items, "Result", msg->result);
			cJSON_AddNumberToObject(items, "Nonce", msg->nonce);
			cJSON_AddItemToObject(json, "Data", items); 
			cJSON_AddStringToObject(json, "MessageType", "JoinInstanceResult");		
			break;
		case START_GAME:
			cJSON_AddNumberToObject(items, "Rounds", msg->rounds);
			cJSON_AddItemToObject(items, "PlayerInfo", msg->players);
			cJSON_AddItemToObject(json, "Data", items); 
			cJSON_AddStringToObject(json, "MessageType", "StartGame");
			break;
		case START_ROUND:
			cJSON_AddNumberToObject(items, "WordLength", msg->word_length);
			cJSON_AddNumberToObject(items, "Round", msg->round);
			cJSON_AddNumberToObject(items, "RoundsRemaining", msg->rounds_remaining);
			cJSON_AddItemToObject(items, "PlayerInfo", msg->players);
	 		cJSON_AddItemToObject(json, "Data", items); 
			cJSON_AddStringToObject(json, "MessageType", "StartRound");
			break;
		case PROMPT:
			cJSON_AddNumberToObject(items, "WordLength", msg->word_length);
			cJSON_AddStringToObject(items, "Name", msg->name);
			cJSON_AddNumberToObject(items, "GuessNumber", msg->guess_number);
			cJSON_AddItemToObject(json, "Data", items); 
			cJSON_AddStringToObject(json, "MessageType", "PromptForGuess");
			break;
		case GUESS:
			cJSON_AddStringToObject(items, "Guess", msg->guess);
			cJSON_AddStringToObject(items, "Name", msg->name);
			cJSON_AddItemToObject(json, "Data", items); 
			cJSON_AddStringToObject(json, "MessageType", "Guess");
			break;
		case GUESS_RESPONSE:
			cJSON_AddStringToObject(items, "Guess", msg->guess);
			cJSON_AddStringToObject(items, "Name", msg->name);
			cJSON_AddStringToObject(items, "Accepted", msg->accepted);
			cJSON_AddItemToObject(json, "Data", items); 
			cJSON_AddStringToObject(json, "MessageType", "GuessResponse");
			break;
		case GUESS_RESULT:
			cJSON_AddStringToObject(items, "Winner", "Maybe");
			cJSON_AddItemToObject(items, "PlayerInfo", msg->players);
			cJSON_AddItemToObject(json, "Data", items);
			cJSON_AddStringToObject(json, "MessageType", "GuessResult");
			break;
		case END_ROUND:
			cJSON_AddNumberToObject(items, "RoundsRemaining", msg->rounds_remaining);
			cJSON_AddItemToObject(items, "PlayerInfo", msg->players);
			cJSON_AddItemToObject(json, "Data", items); 
			cJSON_AddStringToObject(json, "MessageType", "EndRound");
			break;
		case END_GAME:
			cJSON_AddStringToObject(items, "WinnerName", msg->winnerName);
			cJSON_AddItemToObject(items, "PlayerInfo", msg->players);
			cJSON_AddItemToObject(json, "Data", items); 
			cJSON_AddStringToObject(json, "MessageType", "EndGame");
			break;	
		case PLAY_AGAIN:
			cJSON_AddStringToObject(json, "MessageType", "PlayAgain");
			break;
		case STOP:
			cJSON_AddStringToObject(json, "MessageType", "Stop");
			break;

		default:
			puts("bad type");
			return NULL;
	}

	char *str = cJSON_Print(json);
	cJSON_Delete(json);

	return str;
}

void get_message_type(char *object, Message *msg) {
	*object = tolower(object[0]);

	// JOIN
	if(!strcmp(object, "join")) {
		msg->type = JOIN;
	}

	// CHAT
	if(!strcmp(object, "chat")) {

		msg->type = CHAT;
	}
	
	// JOIN_RESULT
	if(!strcmp(object, "joinResult")) {
		msg->type = JOIN_RESULT;
	}

	// JOIN_INSTANCE
	if(!strcmp(object, "joinInstance")) {
		msg->type = JOIN_INSTANCE;
	}

	// JOIN_INSTANCE_RESULT
	if(!strcmp(object, "joinInstanceResult")) {
		msg->type = JOIN_INSTANCE_RESULT;
	}

	// START_INSTANCE
	if(!strcmp(object, "startInstance")) {
		msg->type = START_INSTANCE;
	}

	// START_GAME
	if(!strcmp(object, "startGame")) {
		msg->type = START_GAME;
	}

	// START_ROUND
	if(!strcmp(object, "startRound")) {
		msg->type = START_ROUND;
	}

	// PROMPT
	if(!strcmp(object, "promptForGuess")) {
		msg->type = PROMPT;
	}

	// GUESS
	if(!strcmp(object, "guess")) {
		msg->type = GUESS;
	}

	// GUESS_RESPONSE
	if(!strcmp(object, "guessResponse")) {
		msg->type = GUESS_RESPONSE;
	}

	// GUESS_RESULT
	if(!strcmp(object, "guessResult")) {
		msg->type = GUESS_RESULT;
	}

	// END_ROUND
	if(!strcmp(object, "endRound")) {
		msg->type = END_ROUND;
	}

	// END_GAME
	if(!strcmp(object, "endGame")) {
		msg->type = END_GAME;
	}

	// PLAY_AGAIN
	if(!strcmp(object, "playAgain")) {
		msg->type = PLAY_AGAIN;
	}
	
	// STOP
	if(!strcmp(object, "stop")) {
		msg->type = STOP;
	}

}

Message *message_from_command(char *commands, char *name) {
	Message *msg = calloc(1, sizeof(Message));
	char *cmd = strtok(commands, " \n");

	if (!cmd) return NULL;

	*cmd = tolower(cmd[0]);

	if (!strcmp(cmd, "join")) {
		strcpy(msg->name, name);
		strcpy(msg->client, CLIENT);
		msg->type = JOIN;
	
	} else if (!strcmp(cmd, "chat")) {

		char *text = strtok(NULL, "\n");
		strcpy(msg->name, name);
		strcpy(msg->text, text);
		msg->type = CHAT;

	} else if (!strcmp(cmd, "joinResult")) {

		char *result = strtok(NULL, "\n");
		strcpy(msg->name, name);
		strcpy(msg->result, result);
		msg->type = JOIN_RESULT;


	} else if (!strcmp(cmd, "startGame")) {
		msg->rounds = atoi(strtok(NULL, "\n"));
		msg->type = START_GAME;

	} else if (!strcmp(cmd, "startRound")) {
		msg->word_length = atoi(strtok(NULL, " "));
		msg->round = atoi(strtok(NULL, " "));
		msg->rounds_remaining = atoi(strtok(NULL, " "));
		msg->type = START_ROUND;

	} else if (!strcmp(cmd, "prompt")) {
		msg->word_length = atoi(strtok(NULL, " "));
		strcpy(msg->name, name);
		msg->guess_number = atoi(strtok(NULL, " "));
		msg->type = PROMPT;

	} else if (!strcmp(cmd, "guess")) {
		strcpy(msg->name, name);
		char *guess = strtok(NULL, "\n");
		strcpy(msg->guess, guess);
		msg->type = GUESS;

	} else if (!strcmp(cmd, "guessResponse")) {
		strcpy(msg->name, name);
		char *guess = strtok(NULL, " ");
		strcpy(msg->guess, guess);
		char *accepted = strtok(NULL, "\n");
		strcpy(msg->accepted, accepted);
		msg->type = GUESS_RESPONSE;

	} else if (!strcmp(cmd, "guessResult")) {
		msg->type = GUESS_RESULT;

	} else if (!strcmp(cmd, "endRound")) {
		msg->rounds_remaining = atoi(strtok(NULL, "\n"));
		msg->type = END_ROUND;

	} else if (!strcmp(cmd, "endGame")) {
		char *winner = strtok(NULL, "\n");
		strcpy(msg->winnerName, winner);
		msg->type = END_GAME;

	} else if (!strcmp(cmd, "playAgain")) {
		msg->type = PLAY_AGAIN;

	} else if (!strcmp(cmd, "quit")) {
		msg->type = STOP;

	} else {
		return NULL;
	}

	return msg;


}

Message *message_from_json(char *json) {
	Message *msg = calloc(1, sizeof(Message));

	cJSON *message = cJSON_Parse(json);
	cJSON *item = cJSON_GetObjectItem(message, "MessageType");
	if (!item) return NULL;
	if (item && item->valuestring) { 
		get_message_type(item->valuestring, msg);
	}
	item = cJSON_GetObjectItem(message, "Data");

	cJSON *value = NULL;

	value = cJSON_GetObjectItem(item, "name");
	if (value && value->valuestring) {
		strcpy(msg->name, value->valuestring);	
	}

	value = cJSON_GetObjectItem(item, "client");
	if (value && value->valuestring) {
		strcpy(msg->client, value->valuestring);	
	}

	value = cJSON_GetObjectItem(item, "guess");
	if (value && value->valuestring) {
		strcpy(msg->guess, value->valuestring);	
	}

	value = cJSON_GetObjectItem(item, "text");
	if (value && value->valuestring) {
		strcpy(msg->text, value->valuestring);	
	}
	
	value = cJSON_GetObjectItem(item, "result");
	if (value && value->valuestring) {
		if(strstr(value->valuestring, "es")) {
			strcpy(msg->result, "Yes");
		} else {
			strcpy(msg->result, "No");
		}	
	}

	value = cJSON_GetObjectItem(item, "client");
	if (value && value->valuestring) {
		strcpy(msg->client, value->valuestring);	
	}

	value = cJSON_GetObjectItem(item, "server");
	if (value && value->valuestring) {
		strcpy(msg->server, value->valuestring);	
	}

	value = cJSON_GetObjectItem(item, "port");
	if (value && value->valuestring) {
		strcpy(msg->port, value->valuestring);	
	}
	
	value = cJSON_GetObjectItem(item, "nonce");
	if (value) {
		msg->nonce = value->valueint;
	}
	
	value = cJSON_GetObjectItem(item, "number");
	if (value) {
		msg->number = value->valueint;	
	}
	
	value = cJSON_GetObjectItem(item, "players");
	if (value) {
		msg->players = value;	
	}
	
	value = cJSON_GetObjectItem(item, "wordlength");
	if (value) {
		msg->word_length = value->valueint;	
	}
	
	value = cJSON_GetObjectItem(item, "round");
	if (value) {
		msg->round = value->valueint;	
	}
	
	value = cJSON_GetObjectItem(item, "roundsRemaining");
	if (value) {
		msg->rounds_remaining = value->valueint;	
	}
	
	value = cJSON_GetObjectItem(item, "guessNumber");
	if (value) {
		msg->guess_number = value->valueint;	
	}
	
	value = cJSON_GetObjectItem(item, "accepted");
	if (value) {
		if(strstr(value->valuestring, "es")) {
			strcpy(msg->accepted, "Yes");
		} else {
			strcpy(msg->accepted, "No");;
		}		
	}
	
	value = cJSON_GetObjectItem(item, "winner");
	if (value) {
		if(strstr(value->valuestring, "es")) {
			strcpy(msg->winner, "Yes");
		} else {
			strcpy(msg->winner, "No");
		}	
	}

	value = cJSON_GetObjectItem(item, "winnerName");
	if (value && value->valuestring) {
		strcpy(msg->winnerName, value->valuestring);	
	}


	value = cJSON_GetObjectItem(item, "PlayerInfo");
	int s = cJSON_GetArraySize(value);
	if (s) {
		msg->players = cJSON_Duplicate(value, 1);
	}
	cJSON_Delete(message);

	return msg;
}

void print_info(Message *msg, Type type) {
	cJSON *value = NULL;
	cJSON *array = msg->players;
	cJSON *index = NULL;


	if (type == START_GAME) {
		puts("players in game:");	
		cJSON_ArrayForEach(index, array) {
			value = cJSON_GetObjectItem(index, "Name");
			printf("Name: %s ", value->valuestring);
			value = cJSON_GetObjectItem(index, "Number");
			printf("id: %d\n", value->valueint);
		}

	} else if (type == START_ROUND) {
		puts("Players Scores");
		cJSON_ArrayForEach(index, array) {
			value = cJSON_GetObjectItem(index, "Name");
			printf("Name: %s ", value->valuestring);
			value = cJSON_GetObjectItem(index, "Number");
			printf("id: %d ", value->valueint);
			value = cJSON_GetObjectItem(index, "Score");
			printf("Score: %d\n", value->valueint);
		}

	} else if (type == GUESS_RESULT) {
		puts("Players Guesses");
		cJSON_ArrayForEach(index, array) {
			value = cJSON_GetObjectItem(index, "Name");
			printf("Name: %s ", value->valuestring);
			value = cJSON_GetObjectItem(index, "Number");
			printf("id: %d ", value->valueint);
			value = cJSON_GetObjectItem(index, "Correct");
			printf("Correct: %s ", value->valuestring);
			//value = cJSON_GetObjectItem(index, "ReceiptTime");
			//printf("Time: %lf ", value->valuedouble);
			value = cJSON_GetObjectItem(index, "Result");
			printf("Result: %s\n", value->valuestring);
		}

	} else if (type == END_ROUND) {
		puts("Players at end of round");
		cJSON_ArrayForEach(index, array) {
			value = cJSON_GetObjectItem(index, "Name");
			printf("Name: %s ", value->valuestring);
			value = cJSON_GetObjectItem(index, "Number");
			printf("id: %d ", value->valueint);
			value = cJSON_GetObjectItem(index, "Winner");
			printf("Winner: %s ", value->valuestring);
			value = cJSON_GetObjectItem(index, "ScoreEarned");
			printf("Score Earned: %d\n", value->valueint);
		}
	} else if (type == END_GAME) {
		puts("Final Results");
		cJSON_ArrayForEach(index, array) {
			value = cJSON_GetObjectItem(index, "Name");
			printf("Name: %s ", value->valuestring);
			value = cJSON_GetObjectItem(index, "Number");
			printf("id: %d ", value->valueint);
			value = cJSON_GetObjectItem(index, "Score");
			printf("Score: %d\n", value->valueint);
		}
	} else {
		
	}
}

