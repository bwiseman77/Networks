#ifndef WORDLE_SERVER_H
#define WORDLE_SERVER_H

#include "wordle/wordle.h"

/* definitions */

#define MAX_PLAYERS 100
#define MAX_WORDS 100
#define MAX_WORD_LENGTH 20

// data for each client
typedef struct Client_info {
	int fd; 			// fd for that client
	int Tscore; 		// Overall score
	int Rscore; 		// Score for that round
	bool inGame; 		// if the client is in game
	bool inLobby; 		// if the client is in lobby
	bool isTurn; 		// if its the clients turn // Might not need 
	char name[256]; 	// Clients name
	int nonce; 			// Clients token
	int number; 		// Clients number
	int guesses; 		// number of guesses
	double time; 		// time guess was received 
	bool correct; 		// if guess is correct
	bool canGuess; 	 	// if the client can guess
	char guess[256]; 	// clients guess
	char byg[256]; 		// the byg response string
	bool joining; 		// if client is leaving to join game
} Client_info; 

// data for the game
typedef struct Game_info {
	bool started; 		// if the game has started 
	int word_length; 	// length of current word
	char *word; 		// ptr to current word from list
	bool winner; 		// if someone guessed the word
	char Win[256]; 		// winners name
	bool over; 			// if game is over 
	int Mplayers; 		// max number of players
	int players; 		// number of players
	int guessed_players;// number of guessed 
	int rounds; 		// number of rounds 
	char *gport; 		// ptr to game port
	bool debug; 		// bool for debug 
	char *hostname;		// name of host
	bool newGame; 		// play a new game
	bool quit; 			// quit game instance
} Game_info;


void send_message(char *, char *, int, bool, bool);
cJSON *get_players(Type type);
int get_words(char W[MAX_WORDS][MAX_WORD_LENGTH], char *);

#endif
