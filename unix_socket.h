#ifndef UNIX_SOCKETS_H
#define UNIX_SOCKETS_H

#include <sys/socket.h>
#include <sys/un.h>

/* Build a UNIX domain socket address structure for 'path', returning
 * it in 'addr'. Returns -1 on success, or 0 on error. */
int unixBuildAddress(const char *path, struct sockaddr_un *addr);

/* Create a UNIX domain socket of type 'type' and connect it
 * to the remote address specified by the 'path'.
 * Return the socket descriptor on success, or -1 on error */
int unixConnect(const char *path, int type);

/* Create stream socket, bound to 'path'. Make the socket a listening
 * socket, with the specified 'backlog'. Return socket descriptor on
 * success, or -1 on error. */
int unixListen(const char *path, int backlog);

/* Create socket of type 'type' bound to 'path'.
 * Return socket descriptor on success, or -1 on error. */
int unixBind(const char *path, int type);

#endif
