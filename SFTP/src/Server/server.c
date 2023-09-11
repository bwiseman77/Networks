#include "server_socket.h"

int main(int argc, char *argv[]) {
	if (argc != 2) {
		printf("usage: %s port\n", argv[0]);
		return EXIT_FAILURE;
	}
	char *port = argv[1];
	int server_fd = socket_listen(port);
	if (server_fd < 0)
		return EXIT_FAILURE;

	printf("Listening on port %s\n", port);

	while (1) {

		puts("\nwaiting for client");
		int fd = socket_accept(server_fd);

		puts("accepted connection");

		// recieve filename size
 		char buffer[BUFSIZ];
		recv(fd, buffer, 16, 0);
		uint16_t name_length = ntohs(atoi(buffer));

		// recieve filename
		recv(fd, buffer, name_length, 0);
		buffer[name_length] = '\0';

		printf("fetching file: %s\n", buffer);

		// send file size
		FILE *fp = fopen(buffer, "r");
		if (fp == NULL) {
			fprintf(stderr, "ERROR: Requested file not found\n");
			sprintf(buffer, "%32u", 0);
			send(fd, buffer, 32, 0);
			continue;
		}
		fseek(fp, 0, SEEK_END);
		sprintf(buffer, "%u\n", htonl(ftell(fp)));
		send(fd, buffer, 32, 0);

		// send file data
		fseek(fp, 0, SEEK_SET);
		size_t bytes, byte_total = 0;
		do {
			bytes = fread(buffer, 1, BUFSIZ, fp);
			send(fd, buffer, bytes, 0);
			byte_total += bytes;
		} while (bytes > 0);

		printf("SUCCESS: %zu bytes sent\n", byte_total);

		close(fd);

	}


}
