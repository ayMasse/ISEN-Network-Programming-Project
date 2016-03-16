#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include "utility.h"
#include "inet_socket.h"

#define BUFFER_SIZE 10

int main()
{
    int listen_fd = inetListen("8080", 5, NULL);
    if (listen_fd == -1) {
        errExit("inetListen");
    }


}
