# Networks

Repo for network projects - feel free to ask cuz ngl some of the projects were not very descriptive)

## P1. Simple File Transfer 

Build a client/Server to send and request files.

Client should connect to server and be able to send files as well as request them.

Server should accept connections to then receive and save a file or rend back the requested file. If the file does not exist, send an error message.

Maybe add some good Distysys to make robust

Key thing is that messages need to begin with how long the rest of the message (and length of file) will be, and convert to same byte ordering using ntohs (or similar, look at beejs).

Also remember sockets from systems, actually reading beejs helps understand what the structs are actually doing and why.

## P2. Client/Server Calendar Application

Using a standard API, develop a client / server-based calendar application. This means that diff clients should be able to connect to diff servers based on a well-defined json based API.

The application is based on events that the client adds, edits, or removes. Each event can have the following, but doesnt need a description or location
```
{
  "name":"name of the event",
  "date":"date of the event",
  "time":"time of the event",
  "duration":"duration of event",
  "description":"short description of event",
  "location":"location of event"
}
```

The events should be persistent, or rather if the server crashes, the events should be able to be read if a new server is started (in same env)

look into cJson for a nice and easy C json lib

## Multi-Player Wordle

## Wireshark

## Links:
[Beejs Guide to Network Programming](https://beej.us/guide/bgnet/)

[Linux Networking](https://tldp.org/LDP/intro-linux/html/chap_10.html)

[Textbook](https://book.systemsapproach.org/foundation/problem.html)

[Wireshark Download](https://www.wireshark.org/)
