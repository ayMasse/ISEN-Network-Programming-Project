#include <sys/socket.h>
#include <sys/un.h>
#include <fcntl.h>

int send_fd(int socket_fd, int fd);
int recv_fd(int socket_fd, int* fd);
