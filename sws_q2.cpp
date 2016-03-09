#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <sys/sendfile.h>

#include "utility.h"
#include "inet_socket.h"

#define BUFFER_SIZE 10
#define EPOLL_SIZE 10
#define MAX_EVENTS 10

int main()
{
    int listen_fd = inetListen("8080", 5, NULL);
    if (listen_fd == -1)
        errExit("inetListen");

    // Turn on non-blocking mode on stdin
    int flags = fcntl(STDIN_FILENO, F_GETFL);
    if (fcntl(STDIN_FILENO, F_SETFL, flags|O_NONBLOCK) == -1)
        errExit("stdin non block");

    // Turn on non-blocking mode on the passive socket
    flags = fcntl(listen_fd, F_GETFL);
    if (fcntl(listen_fd, F_SETFL, flags|O_NONBLOCK) == -1)
        errExit("listen fd non block");

    // Initialize the epoll instance
    int epfd;
    epfd = epoll_create(EPOLL_SIZE);
    if (epfd == -1)
        errExit("epoll_create");

    struct epoll_event ev;
    ev.events = EPOLLIN; // event : data can be read

    // add listen_fd to the interest list
    ev.data.fd = listen_fd;
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, listen_fd, &ev) == -1)
        errExit("epoll_ctl");

    // add STDIN to the interest list
    ev.data.fd = STDIN_FILENO;
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, STDIN_FILENO, &ev) == -1)
        errExit("epoll_ctl");

    char buffer[BUFFER_SIZE];
    // the event list used with epoll_wait()
    struct epoll_event evlist[MAX_EVENTS];

    char x = 0;
    while (x != 'q') {
        int nb_fd_ready = epoll_wait(epfd, evlist, MAX_EVENTS, -1);

        if (nb_fd_ready == -1) {
            if (errno == EINTR) // restart if interrupted by signal
                continue;
            else
                errExit("epoll");
        }

        for (int i = 0 ; i < nb_fd_ready ; ++i) {
            int fd = evlist[i].data.fd;
            if (fd == STDIN_FILENO) {
                // Close the server by typing the letter q
                if ((read(STDIN_FILENO, &x, 1) == 1) && (x == 'q'))
                    break;
            } else if (fd == listen_fd) {
                int client_fd = accept(listen_fd, NULL, NULL);

                // The accept system call returns an error
                // that is not related to the non-blocking behavior
                // this error should be handled more gracefully
                if (client_fd == -1 && errno != EWOULDBLOCK)
                    errExit("accept");

                // There is a new connection
                if (client_fd != -1) {
                    printf("Accept a new connection...\n");
                    flags = fcntl(client_fd, F_GETFL);
                    if (fcntl(client_fd, F_SETFL, flags|O_NONBLOCK) == -1) {
                        close(client_fd);
                    } else {
                        ev.data.fd = client_fd;
                        if (epoll_ctl(epfd, EPOLL_CTL_ADD, client_fd, &ev) == -1)
                            close(client_fd);
                    }
                }
            } else { // a client fd is ready
                /*int num_read = read(fd, buffer, BUFFER_SIZE);

                if (num_read == -1 && errno != EWOULDBLOCK)
                    errExit("read");

                if (num_read == 0) { // The client closed the connection
                    printf("Closed connection...\n");
                    close(fd);
                } else if (num_read > 0) { // Something was read from client_fd
                    buffer[num_read] = 0; // end of string
                    printf("%d(%d) => %s\n", fd, num_read, buffer);
                }*/
            }
        }
    }

    close(listen_fd);
    return EXIT_SUCCESS;
}
