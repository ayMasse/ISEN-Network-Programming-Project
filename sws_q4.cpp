#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/sendfile.h>
#include "utility.h"
#include "inet_socket.h"
#define BUF_SIZE 300

int main()
{
	int listen_fd = inetListen("8080", 5, NULL);

	if (listen_fd == -1)
		errExit("inetListen");

	// Turn on non-blocking mode on the passive socket
	int flags = fcntl(listen_fd, F_GETFL);
	if (fcntl(listen_fd, F_SETFL, flags | O_NONBLOCK) == -1)
		errExit("listen fd non block");

	for (;; ) {
		int client_fd = accept(listen_fd, NULL, NULL);
		if (client_fd == -1 && errno != EWOULDBLOCK)
			errExit("accept");
		if (client_fd != -1) {
			char buf[BUF_SIZE];
			printf("Accept a new connection... \n");
			int numRead = read(client_fd, buf, BUF_SIZE);
			if (write(STDOUT_FILENO, buf, numRead) != numRead)
				errExit("partial/failed write");
			int fd = open("./index.html", O_RDONLY);
			if (fd < 0)
				errExit("opening file failed");
			numRead = read(fd, buf, 300);
			char headers[BUF_SIZE];
			ssize_t nbBytes = snprintf(headers, BUF_SIZE, "HTTP/1.x 200 OK\r\nContent-Type: text/html;\r\nContent-Length: %i\r\ncharset=UTF-8\r\n\r\n", numRead);
			if (numRead == -1)
				errExit("reading file failed");
			write(client_fd, headers, nbBytes);
			sendfile(client_fd, fd, NULL, BUF_SIZE);
			close(client_fd);
			printf("Closed connection... \n");
		}
	}
	close(listen_fd);
}
