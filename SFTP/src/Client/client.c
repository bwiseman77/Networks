#include "client_socket.h"
#include <sys/time.h>

int main(int argc, char *argv[]) {
	puts("welcome client");
	if (argc != 4) {
		printf("usage: %s host port filename\n", argv[0]);
		return EXIT_FAILURE;
	}
	char *host = argv[1];
	char *port = argv[2];
	char *msg = argv[3];

	// begin time
	struct timeval start;
	gettimeofday(&start, NULL);

	// connect to server
	int fd = socket_dial(host, port);
	if (fd < 0) {
		return EXIT_FAILURE;
	}

	// send length of filename
	uint16_t name_length = strlen(msg);
	char buffer[BUFSIZ];
	sprintf(buffer, "%16hu", htons(name_length));
	send(fd, buffer, 16, 0);

	// send name of file
	size_t bytes = send(fd, msg, name_length, 0);
	// recieve file size
	recv(fd, buffer, 32, 0);
	uint32_t file_length = ntohl(atol(buffer));
	if (file_length == 0) {
		puts("ERROR: something went wrong");
		close(fd);
		return EXIT_FAILURE;
	}

	FILE *fp = fopen(msg, "w");
	size_t byte_total = 0;
	do {
		bytes = recv(fd, buffer, BUFSIZ, 0);
		fwrite(buffer, 1, bytes, fp);
		byte_total += bytes;
	} while (bytes > 0);

	// end time
	struct timeval end;
	gettimeofday(&end, NULL);

	// printf("SUCCESS: %zu bytes recieved\n", byte_total);
	double delta = end.tv_sec - start.tv_sec + 0.000001 * (end.tv_usec - start.tv_usec);
	printf("%zu bytes transferred over %lf seconds for a speed of %lf MB/sec.\n", byte_total, delta, byte_total/(1000000*delta));

	close(fd);


	return 0;
}
