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
    
}
