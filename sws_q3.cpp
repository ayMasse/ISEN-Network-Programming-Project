#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <unistd.h>
#include "utility.h"
#include "inet_socket.h"
#define BUF_SIZE 300

int main()
{
    int listen_fd = inetListen("8080", 5, NULL);
    if (listen_fd == -1){
        errExit("inetListen");
    }
	
	// Turn on non-blocking mode on the passive socket
    int flags = fcntl(listen_fd, F_GETFL);
    if (fcntl(listen_fd, F_SETFL, flags|O_NONBLOCK) == -1){
        errExit("listen fd non block");
	}
	
	for(;;){
		int client_fd = accept(listen_fd,NULL,NULL);
		if(client_fd == -1 && errno != EWOULDBLOCK){
			errExit("accept");
		}
		if(client_fd != -1){
			char buf[BUF_SIZE];
			printf("Accept a new connection... \n");
			int numRead = read(client_fd, buf, BUF_SIZE);
			if(write(STDOUT_FILENO,buf,numRead) != numRead)
			{
				errExit("partial/failed write");
			}
			int fd = open("index.html",O_RDONLY);
			if(fd < 0){
				errExit("opening file failed");
			}
			numRead = read(fd,buf,300);
			char headers[] = "HTTP/1.x 200 OK\nContent-Type: text/html; charset=UTF-8\n\n";
			ssize_t nbBytes = sizeof(headers) / sizeof(char);
			if(numRead == -1){
				errExit("reading file failed");
			}
			write(client_fd,headers,nbBytes);
			write(client_fd,buf,numRead);
			close(client_fd);
			printf("Closed connection... \n");
		}
	}
	close(listen_fd);
}
