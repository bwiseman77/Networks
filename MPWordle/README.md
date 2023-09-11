# Multiplayer Wordle

## Set up

start server: `/mpwordleserver -[options]`

options:
- np X -> Number of players (default is 2)
- lp X -> Lobby port (your choice on default)
- pp X -> The starting number for play ports to use (your choice on default)
- nr X -> Number of rounds (default is 3)
- d DFile -> Dictionary of valid words where the words are listed on a line by line basis.  Words must be three or more letters and less than ten letters  (your choice on default - you should provide a file in your server)
- dbg -> If this argument is present, your server should print a brief debug message for any message received or sent. 

start client: `/mpwordle -name $name -server $Server -port $port`

## Play game

To make a guess, type `guess $guess`

To chat to other players, type `chat $message`

To quit, hit `CTRL-C`

## Extensions

Clients can quit and game continues

Clients can quit lobby, making room for new players

Slightly more complex scoring

Server "chats" to clients if one leaves
