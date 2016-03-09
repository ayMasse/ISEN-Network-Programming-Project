#include "unix_socket.h"       /* Declares functions defined here */
#include "utility.h"

#include <errno.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>

int
unixBuildAddress(const char *path, struct sockaddr_un *addr)
{
    if (addr == NULL || path == NULL ||
            strlen(path) >= sizeof(addr->sun_path) - 1) {
        errno = EINVAL;
        return -1;
    }

    memset(addr, 0, sizeof(struct sockaddr_un));
    addr->sun_family = AF_UNIX;
    if (strlen(path) < sizeof(addr->sun_path)) {
        strncpy(addr->sun_path, path, sizeof(addr->sun_path) - 1);
        return 0;
    } else {
        errno = ENAMETOOLONG;
        return -1;
    }
}

int
unixConnect(const char *path, int type)
{
    int sd, savedErrno;
    struct sockaddr_un addr;

    if (unixBuildAddress(path, &addr) == -1)
        return -1;

    sd = socket(AF_UNIX, type, 0);
    if (sd == -1)
        return -1;

    if (connect(sd, (struct sockaddr *) &addr,
                sizeof(struct sockaddr_un)) == -1) {
        savedErrno = errno;
        close(sd);                      /* Might change 'errno' */
        errno = savedErrno;
        return -1;
    }

    return sd;
}

/* Create a UNIX domain socket and bind it to 'path'. If 'doListen'
 * is true, then call listen() with specified 'backlog'.
 * Return the socket descriptor on success, or -1 on error. */
static int              /* Public interfaces: unixBind() and unixListen() */
unixPassiveSocket(const char *path, int type, bool doListen, int backlog)
{
    int sd, savedErrno;
    struct sockaddr_un addr;

    if (unixBuildAddress(path, &addr) == -1)
        return -1;

    sd = socket(AF_UNIX, type, 0);
    if (sd == -1)
        return -1;

    if (bind(sd, (struct sockaddr *) &addr, sizeof(struct sockaddr_un)) == -1) {
        savedErrno = errno;
        close(sd);                      /* Might change 'errno' */
        errno = savedErrno;
        return -1;
    }

    if (doListen) {
        if (listen(sd, backlog) == -1) {
            savedErrno = errno;
            close(sd);                  /* Might change 'errno' */
            errno = savedErrno;
            return -1;
        }
    }

    return sd;
}

int
unixListen(const char *path, int backlog)
{
    return unixPassiveSocket(path, SOCK_STREAM, true, backlog);
}

int
unixBind(const char *path, int type)
{
    return unixPassiveSocket(path, type, false, 0);
}


