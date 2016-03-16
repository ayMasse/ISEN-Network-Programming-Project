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
		char buf[BUF_SIZE];
		int client_fd = accept(listen_fd,NULL,NULL);
		if(client_fd == -1 && errno != EWOULDBLOCK){
			errExit("accept");
		}
		if(client_fd != -1){
			printf("Accept a new connection... \n");
			int numRead = read(client_fd, buf, BUF_SIZE);
			if(write(STDOUT_FILENO,buf,numRead) != numRead)
			{
				errExit("partial/failed write");
			}
			close(client_fd);
			printf("Closed connection... \n");
		}
	}
	close(listen_fd);
}
