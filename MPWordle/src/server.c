#include "wordle/wordle_server.h"
#include "wordle/server_socket.h"
#include <stdlib.h>

#include <time.h>
#include <sys/time.h>

/**
 * Return current timestamp as a double.
 *
 * Utilizes gettimeofday for precision (accounting for seconds and
 * microseconds) and falls back to time (only account for seconds) if that
 * fails.
 **/
double timestamp() {
    // set timeval struct
    struct timeval tv;

    // check of fails
    if(gettimeofday(&tv, NULL) < 0) {
       return time(NULL);
    }

    return tv.tv_sec/1.0 + ( tv.tv_usec / 1000000.0 );
}

/* Globals */

pthread_mutex_t Lock = PTHREAD_MUTEX_INITIALIZER;
pthread_t threads[MAX_PLAYERS];
Client_info Clients[MAX_PLAYERS];
Game_info Game = {.started = false, .winner = false, .guessed_players = 0, .players=0};
int Players = 0;
int players_in_game = 0;
pthread_t game;
char Words[MAX_WORDS][MAX_WORD_LENGTH];
int NumWords = 0;

/* Thread Functions */

void *client_thread(void *arg) {
	/* get client info struct */
	Client_info *client = (Client_info *)arg;
	char buffer[BUFSIZ];

	/* recieve messages, and send to everyone if CHAT */
	while(1) {

		/* if client quits, close fd and mark as not in game */
		if (recv(client->fd, buffer, BUFSIZ, 0) <= 0) {
			printf("client %s quit\n", client->name);
			close(client->fd);
			pthread_mutex_lock(&Lock);
			if (Game.players) {
				Game.players--;

			}
			else if (!client->joining) players_in_game--;
			pthread_mutex_unlock(&Lock);	
			client->inGame = false;
			
			/* send to other clients that they quit */
			for (int i = 0; i < Players; i++) {
				sprintf(buffer, "Chat %s quit the game\n", client->name);
				if (Clients[i].fd != client->fd && client->joining) send_message(buffer, "Server", Clients[i].fd, Game.debug, Clients[i].inGame);	
			}

			return 0;
		}

		/* buffer is json */
		if (Game.debug) printf("from fd %d: %s\n", client->fd, buffer);
		Message *msg = message_from_json(buffer);

		/* send to everyone but sender */
		if (msg->type == CHAT) {
			for (int i = 0; i < Players; i++) {
				/* only send if not sended and ingame */
				if (client->inGame && Clients[i].fd != client->fd ) {
					send(Clients[i].fd, buffer, BUFSIZ, 0);
				}
			}
		}

		/* if message is guess */
		if (msg->type == GUESS) {

			/* check if client can guess and is a valid sized guess */
			if (!Clients[client->nonce].canGuess || strlen(msg->guess) != Game.word_length) {
				sprintf(buffer, "guessResponse %s No\n", msg->guess);
				send_message(buffer, client->name, client->fd, Game.debug, client->inGame);

			} else {
				/* get time recieved */
				double time = timestamp();

				/* increase guesses */
				Clients[client->nonce].guesses++;

				sprintf(buffer, "guessResponse %s Yes\n", msg->guess);
				send_message(buffer, client->name, client->fd, Game.debug, client->inGame);
	
				//printf("%d %s\n", client->nonce, msg->guess);

				/* set up variables */
				char byg[Game.word_length];
				byg[Game.word_length] = '\0';
				char *w = Game.word;
				char *g = msg->guess;
				int score = 0;
				bool letters[26] = {0};

				/* go through word */
				for (int i = 0; i < Game.word_length; i++) {

					/* check green */
					if (w[i] == g[i]) {
						byg[i] = 'G';
						score += 10/Clients[client->nonce].guesses;
						continue;
					}
				
					/* check yellow and black */
					/* also mark of a letter has already been marked */
					bool B = true;
					for (int j = 0; j < Game.word_length; j++) {
						if (w[j] == g[i]) {
							if (!letters[(int) g[i] - 'a']) {
								byg[i] = 'Y';
								letters[(int) g[i] - 'a'] = true;
				
								score += 3/Clients[client->nonce].guesses;
								B = false;
							}
						}
					}

					if (B) {
						byg[i] = 'B';
					}
				}
			
				/* update some values with locks */
				pthread_mutex_lock(&Lock);

				/* if correct guess */
				if (!strcmp(g, w)) {
					Game.winner = true;
					Clients[client->nonce].correct = true;
				}

				/* update guess for client */
				strcpy(Clients[client->nonce].byg, byg);
				Clients[client->nonce].canGuess = false;
				Clients[client->nonce].Rscore += score;
				Clients[client->nonce].time = time;

				Game.guessed_players++;
				pthread_mutex_unlock(&Lock);
			}

		}

		if (msg->type == PLAY_AGAIN) {
			
		} 

		if (msg->type == STOP) {


		}

		free(msg);
	}
}	

void *game_thread(void * arg) {
	int game_fd = socket_listen(Game.gport);
	char buff[BUFSIZ];

	/* set up game */
	Game.players = 0;
	Game.word_length = 5;
	Game.winner = false;
	Game.guessed_players = 0;
	Game.started = true;

	int new_fd;
	/* collect players */
	while(Game.players < Game.Mplayers) { // Game.Mplayers
		new_fd = socket_accept(game_fd);
		recv(new_fd, buff, BUFSIZ, 0);
		Message *msg = message_from_json(buff);
		char *s = message_to_json(msg, msg->type);
		if (Game.debug) printf("recv: %s\n", s);

		/* if join instance */
		if (msg->type == JOIN_INSTANCE) {
			for (int i = 0; i < Players; i++) {
				/* check if nonce matches */
				printf("%d %d\n", msg->nonce, Clients[i].nonce);
				if (msg->nonce == Clients[i].nonce) {
					pthread_mutex_lock(&Lock);
					Game.players++;
					pthread_mutex_unlock(&Lock);
					Clients[i].fd = new_fd;
					Clients[i].inGame = true;

					/* ADD TO guesses info array */
					strcpy(Clients[i].name, msg->name);
				    Clients[i].number = msg->nonce;
					Clients[i].correct	= false;
					Clients[i].canGuess = false;						
					
					/* create game thread */
					pthread_create(&threads[i], NULL, client_thread, &Clients[i]);
				}
			}
		}

		printf("%d %d\n", Game.players, Game.Mplayers);

		free(s);
		free(msg);
	}

	/* play game */
	int MaxRounds = Game.rounds;
	int currRound = 1;
	puts("start game");
	/* send start game */
	for (int i = 0; i < Players; i++) {
		sprintf(buff, "startGame %d\n", MaxRounds);
		send_message(buff, NULL, Clients[i].fd, Game.debug, Clients[i].inGame);
		if (Game.debug) printf("send: %s\n", buff);	
	}

	/* rounds */
	while (currRound <= MaxRounds) {

		/* get word */
		srand(time(NULL));
		int num = rand() % NumWords;
		Game.word = Words[num];
		Game.word_length = strlen(Words[num]);

		/* send start round */
		for (int i = 0; i < Players; i++) {
			sprintf(buff, "startRound %d %d %d\n", Game.word_length, currRound, MaxRounds - currRound + 1);
			send_message(buff, NULL, Clients[i].fd, Game.debug, Clients[i].inGame);
			if (Game.debug) printf("send: %s\n", buff);	

		}
		
		Game.winner = false;
		/* curr round */

		while(!Game.winner && Game.players) {
				
			/* prompt for guess */
			for (int i = 0; i < Players; i++) {
				Clients[i].canGuess = true;
				sprintf(buff, "prompt %d %d", Game.word_length, Clients[i].guesses);
				send_message(buff, Clients[i].name, Clients[i].fd, Game.debug, Clients[i].inGame);
				if (Game.debug) printf("send: %s\n", buff);	
			}

			/* wait for players to guess */
			while(Game.guessed_players < Game.players);
			Game.guessed_players = 0;
	
			// send guess_result
			for (int i = 0; i < Players; i++) {
				sprintf(buff, "guessResult\n");
				send_message(buff, NULL, Clients[i].fd, Game.debug, Clients[i].inGame);
				if (Game.debug) printf("send: %s\n", buff);	
			}
		}

		/* end round */
		/*  update score */
		for (int i = 0; i < Players; i++) {
			Clients[i].Tscore += Clients[i].Rscore;
			Clients[i].guesses = 0;
		}

		for (int i = 0; i < Players; i++) {
			sprintf(buff, "endRound %d\n", MaxRounds - currRound);
			send_message(buff, NULL, Clients[i].fd, Game.debug, Clients[i].inGame);
			if (Game.debug) printf("send: %s\n", buff);	
		}

		for (int i = 0; i < Players; i++) {
			Clients[i].correct = false;
			Clients[i].Rscore = 0;
		}

		currRound++;		
	}

	/* winner */
	int w = 0;
	for (int i = 0; i < Players; i++) {
		if(Clients[i].Tscore > w) {
			w = Clients[i].Tscore;
			strcpy(Game.Win, Clients[i].name);
		}
	}

	for (int i = 0; i < Players; i++) {
		sprintf(buff, "endGame %s\n", Game.Win);
		send_message(buff, NULL, Clients[i].fd, Game.debug, Clients[i].inGame);
	}
	
	puts("game over");
	Game.over = true;
	close(game_fd);
	players_in_game = 0;
	return NULL;
}

int main(int argc, char *argv[]) {

	/* set up variables */
	int num_players = 1;
	int argind = 1;
	char *lport = "42000";
	char *gport = "43000";
	int rounds = 1;
	char *dict = "words.txt";
	bool debug = false;
	int server_fd;
	char buffer[BUFSIZ];

	char hostname[30];
	gethostname(hostname, sizeof(hostname));
	printf("%s\n", hostname);


	/* parse flags */
	while (argind < argc && argv[argind][0] == '-') {
		
		if (!strcmp("-np", argv[argind])) {
			num_players = atoi(argv[++argind]);
		} else if (!strcmp("-lp", argv[argind])) {
			lport = argv[++argind];
		} else if (!strcmp("-pp", argv[argind])) {
			gport = argv[++argind];
		} else if (!strcmp("-nr", argv[argind])) {
			rounds = atoi(argv[++argind]);
		} else if (!strcmp("-d", argv[argind])) {
			dict = argv[++argind];
		} else if (!strcmp("-dbg", argv[argind])) {
			debug = true;
		} else {
			puts("bad flag");
			return 0;
		}

		argind++;
	}

	/* set up server */
	NumWords = get_words(Words, dict);
	Game.gport = gport;
	Game.rounds = rounds;
	Game.debug = debug;
	Game.Mplayers = num_players;

	if ((server_fd = socket_listen(lport)) < 0) { puts("error"); return 0; }
	printf("listening on port %s\n", lport);
	
	/* main loop */
	while (1) {

		/* while game is on */
/*		if (Game.started) {
			if (players_in_game <= 0) {
				break;
			}
		}
*/
		/* gather clients */
		int client_fd = socket_accept(server_fd);	
		
		/* accept clients */
		recv(client_fd, buffer, BUFSIZ, 0);
	
		Message *msg = message_from_json(buffer);
		char *s = message_to_json(msg, msg->type);
		
		/* print debug */
		if (Game.debug) printf("request: %s\n", s);
		
		free(s);
		
		/* check messages */
		if (msg->type == JOIN) {

			/* if game is full */
			if (players_in_game >= num_players) {// might not need 	

				/* send No response to extra players */				
				sprintf(buffer, "joinResult No\n");
				send_message(buffer, msg->name, client_fd, Game.debug, true);

			} else { 
		
				/* send Yes reponse to players */
				sprintf(buffer, "joinResult Yes\n");
				send_message(buffer, msg->name, client_fd, Game.debug, true);

				/* add to list of clients, and start thread */ 
				Clients[Players].fd = client_fd;
				Clients[Players].inGame = true;
				Clients[Players].isTurn = false;
				Clients[Players].correct = false;
				Clients[Players].Tscore = 0;
				Clients[Players].Rscore = 0;
				Clients[Players].nonce = Players;
				Clients[Players].guesses = 0;
				Clients[Players].joining = false;
				strcpy(Clients[Players].name, msg->name);
	
				/* start thread */
				pthread_create(&threads[Players], NULL, client_thread, &Clients[Players]);
	
				free(msg);

				/* add to players */	
				Players++;
				players_in_game++;

				/* start game */
				if (players_in_game >= num_players) {
					
					/* set up Game */
					Game.started = false;
					Game.winner = false;
					Game.over = false;
					Game.guessed_players = 0;

					/* create game thread */
					pthread_create(&game, NULL, game_thread, NULL);

					/* wait until game port is set up */
					while(!Game.started);

					/* send clients to game port */			
					Message start;
					strcpy(start.server, hostname);
					strcpy(start.port, gport);
					start.type = START_INSTANCE;

					/* send to each player */
					for (int i = 0; i < Players; i++) {
						start.nonce = Clients[i].nonce;
						char *s = message_to_json(&start, START_INSTANCE);

						/* only if player in game */
						if (Clients[i].inGame) {
							Clients[i].joining = true;
							if (Game.debug) printf("%s\n", start.server);
							send(Clients[i].fd, s, strlen(s) + 1, 0);
						}

						free(s);
					}

					/* join client lobby threads */
					for (int i = 0; i < Players; i++) {
						pthread_join(threads[i], NULL);
					}
			
				}

			}
		}
	}

	/* join client game hreads */
	for (int i = 0; i < Players; i++) {
		pthread_join(threads[i], NULL);
	}

	/* join game thread */
	pthread_join(game, NULL);

	return 0;
}
