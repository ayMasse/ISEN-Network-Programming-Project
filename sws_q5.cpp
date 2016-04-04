#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/sendfile.h>
#include <sys/stat.h>
#include <signal.h>
#include "utility.h"
#include "inet_socket.h"

#define BUF_SIZE 300
#define LISTEN_PORT "8080"
#define BACKLOG 5

int main()
{
	int listen_fd = inetListen(LISTEN_PORT, BACKLOG, NULL);

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

			char *request = strtok(buf, " ");
			request = strtok(NULL, " ");
			char path[BUF_SIZE];
			snprintf(path, BUF_SIZE, ".%s", request);
			struct stat sb;

			if (stat(path, &sb) == 0 && S_ISDIR(sb.st_mode)) {
				char path2[BUF_SIZE];
				strcpy(path2, path);
				snprintf(path, BUF_SIZE, "%sindex.html", path2);
				printf("PATH = %s\n", path);
			}

			int fd = open(path, O_RDONLY);

			if (fd < 0) {
				char headers[BUF_SIZE];
				ssize_t nbBytes = snprintf(headers, BUF_SIZE, "HTTP/1.x 404 NOT FOUND\r\n\r\n");
				write(client_fd, headers, nbBytes);
			} else {
				struct stat stat_buf;
				fstat(fd, &stat_buf);
				char headers[BUF_SIZE];
				ssize_t nbBytes = snprintf(headers, BUF_SIZE, "HTTP/1.x 200 OK\r\nContent-Type: text/html;\r\nContent-Length: %i\r\ncharset=UTF-8\r\n\r\n", stat_buf.st_size);
				write(client_fd, headers, nbBytes);
				sendfile(client_fd, fd, 0, stat_buf.st_size);
				close(fd);
			}
			close(client_fd);
			printf("Closed connection... \n");
		}
	}
	close(listen_fd);
}
