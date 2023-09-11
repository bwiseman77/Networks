#include "mycal/server_socket.h"

/**
 * Create socket and bind to port
 *
 * @param port 		port to bind to
 * @return fd 		socket file descriptor
 **/
int socket_listen(const char *port) {
	/* set structures */
	struct addrinfo hints, *res, *p;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC; 		// IPv4 or IPv6
	hints.ai_socktype = SOCK_STREAM; 	// TCP
	hints.ai_flags = AI_PASSIVE; 		
	int status;

	/* fill in res struct */
	if ((status = getaddrinfo(NULL, port, &hints, &res)) != 0) {
		fprintf(stderr, "getaddrinfo failed: %s\n", gai_strerror(status));
		return -1;
	}

	/* find fd */
	int fd = -1;
	for (p = res; p != NULL && fd < 0; p = p->ai_next) {
		/* socket */
		if ((fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) < 0) {
			fprintf(stderr, "socket failed: %s\n", strerror(errno));
			continue;
		}

		/* get rid of already in use error */
		int yes=1;
		if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes) == -1) {
			perror("setsockopt");
			return -1;
		}

		/* bind */
		if (bind(fd, p->ai_addr, p->ai_addrlen) < 0) {
			fprintf(stderr, "bind failed: %s\n", strerror(errno));
			close(fd);
			fd = -1;
			continue;
		}

		/* listen */
		if (listen(fd, BACKLOG) < 0) {
			fprintf(stderr, "listen failed: %s\n", strerror(errno));
			close(fd);
			fd = -1;
			continue;
		}
	}

	freeaddrinfo(res);

	return fd;
}

/**
 * Accept new requests
 * 
 * @param server_fd 	server file descriptor
 * @return client_fd 	client file descriptor
 **/
int socket_accept(int server_fd) {
	struct sockaddr client_addr;
	socklen_t addr_size = sizeof(struct sockaddr);

	int client_fd = accept(server_fd, &client_addr, &addr_size);
		
	if (client_fd < 0) {
		fprintf(stderr, "accept failed: %s\n", strerror(errno));
		return -1;
	}

	return client_fd;
}
