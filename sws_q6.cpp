#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/sendfile.h>
#include <sys/stat.h>
#include <string>
#include "utility.h"
#include "inet_socket.h"
#include <sys/epoll.h>
#include <signal.h>
#define BUF_SIZE 300
#define EPOLL_SIZE 5000
#define MAX_EVENTS 5000

int main()
{
	int listen_fd = inetListen("8080", 5000, NULL);

	if (listen_fd == -1)
		errExit("inetListen");

  	signal(SIGPIPE, SIG_IGN);

	// Turn on non-blocking mode on the passive socket
	int flags = fcntl(listen_fd, F_GETFL);
	if (fcntl(listen_fd, F_SETFL, flags | O_NONBLOCK) == -1)
		errExit("listen fd non block");

	// Initialize the epoll instance
	int epfd;
	epfd = epoll_create(EPOLL_SIZE);
	if (epfd == -1)
		errExit("epoll_create");

	struct epoll_event ev;
	ev.events = EPOLLIN; // event : data can be read

	// add listen_fd to the interest list
  	ev.data = {};
	ev.data.fd = listen_fd;
	if (epoll_ctl(epfd, EPOLL_CTL_ADD, listen_fd, &ev) == -1)
		errExit("epoll_ctl");

	// the event list used with epoll_wait()
	struct epoll_event evlist[MAX_EVENTS];

	for (;; ) {
		int nb_fd_ready = epoll_wait(epfd, evlist, MAX_EVENTS, 0);
		if (nb_fd_ready == -1) {
			if (errno == EINTR) // restart if interrupted by signal
				continue;
			else
				errExit("epoll");
		}
		for (int i = 0; i < nb_fd_ready; ++i) {
			int fd_ready = evlist[i].data.fd;
			if (fd_ready == listen_fd) {
				int client_fd = accept(listen_fd, NULL, NULL);
				printf("Accept a new connection... \n");
				flags = fcntl(client_fd, F_GETFL);
				if (fcntl(client_fd, F_SETFL, flags | O_NONBLOCK) == -1) {
					close(client_fd);
					printf("Closed connection...\n");
				} else {
					ev.data.fd = client_fd;
					if (epoll_ctl(epfd, EPOLL_CTL_ADD, client_fd, &ev) == -1) {
						close(client_fd);
						printf("Closed connection...\n");
					}
				}
			}
			else {
				char buf[BUF_SIZE];
				int numRead = read(fd_ready, buf, BUF_SIZE);
				if(numRead == 0){
					close(fd_ready);
					printf("Closed connection...\n");
				}
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
					ssize_t nbBytes = snprintf(headers, BUF_SIZE, "HTTP/1.x 404 NOT FOUND\r\nConnection: keep-alive\r\n\r\n");
          write(fd_ready, headers, nbBytes);
				} else {
					struct stat stat_buf;
					fstat(fd, &stat_buf);
					char headers[BUF_SIZE];
					ssize_t nbBytes = snprintf(headers, BUF_SIZE, "HTTP/1.x 200 OK\r\nContent-Type: text/html;\r\nContent-Length: %i\r\ncharset=UTF-8\r\nConnection: keep-alive\r\n\r\n", stat_buf.st_size);
          write(fd_ready, headers, nbBytes);
					sendfile(fd_ready, fd, 0, stat_buf.st_size);
				}
			}
		}
	}
	close(listen_fd);
	return EXIT_SUCCESS;
}
