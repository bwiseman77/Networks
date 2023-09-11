#include "mycal/client_socket.h"

/** 
 * Get sockaddr for IPv4 or IPv6
 *
 * @param  sa 	socket addr to convert
 * @return the converted socket addr
 **/
void *get_in_addr(struct sockaddr *sa) {
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in *)sa)->sin_addr);
	} else {
		return &(((struct sockaddr_in6 *)sa)->sin6_addr);
	}
}

/**
 * Create socket for specified port and host
 *
 * @param 	host 		host string to connect to
 * @param 	port 		port string to connect to
 * @return  client_file 			sockect file pointer	
 **/
int socket_dial(const char* host, const char* port) {
	/* set structures */
	struct addrinfo hints, *res, *p;
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC; 	// either Iv4 or Iv6
	hints.ai_socktype = SOCK_STREAM; // TCP
	int status;

	/* fill in res struct */
	if ((status = getaddrinfo(host, port, &hints, &res)) != 0) {
		fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
		return -1;
	}

	/* set up gauntlet: set fd to -1, loop until end of linked list or 
	 * connected (fd > -1)
	 */
	int fd = -1;
	for (p = res; p != NULL && fd < 0; p = p->ai_next) {
		/* create socket */
		if ((fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) < 0) {
			fprintf(stderr, "socket failed: %s\n", strerror(errno));
			continue;
		}

		/* connect to host */
		if (connect(fd, p->ai_addr, p->ai_addrlen) < 0) {
			fprintf(stderr, "connect failed: %s\n", strerror(errno));
			close(fd);
			fd = -1;
			continue;
		}

		/* print connecting message */
		if (fd != -1) {
			char ipstr[INET6_ADDRSTRLEN];
			inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), ipstr, sizeof ipstr);
			printf("connecting to %s\n", ipstr);
		}
	}

	/* free struct */
	freeaddrinfo(res);

	return fd;
}
