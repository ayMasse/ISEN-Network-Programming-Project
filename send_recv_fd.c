#include <stdlib.h>

#include "send_recv_fd.h"
#include "utility.h"

int send_fd(int socket_fd, int fd) {
    struct msghdr msgh;
    struct iovec iov;
    int data;
    ssize_t ns;

    /* Allocate a char array of suitable size to hold the ancillary data.
       However, since this buffer is in reality a 'struct cmsghdr', use a
       union to ensure that it is aligned as required for that structure. */
    union {
        struct cmsghdr cmh;
        char   control[CMSG_SPACE(sizeof(int))];
        /* Space large enough to hold an 'int' */
    } control_un;
    struct cmsghdr *cmhp;

    /* On Linux, we must transmit at least 1 byte of real data in
       order to send ancillary data */

    msgh.msg_iov = &iov;
    msgh.msg_iovlen = 1;
    iov.iov_base = &data;
    iov.iov_len = sizeof(int);
    data = 12345;

    /* We don't need to specify destination address, because we use
       connect() below */

    msgh.msg_name = NULL;
    msgh.msg_namelen = 0;

    msgh.msg_control = control_un.control;
    msgh.msg_controllen = sizeof(control_un.control);

    /* Set message header to describe ancillary data that we want to send */

    cmhp = CMSG_FIRSTHDR(&msgh);
    cmhp->cmsg_len = CMSG_LEN(sizeof(int));
    cmhp->cmsg_level = SOL_SOCKET;
    cmhp->cmsg_type = SCM_RIGHTS;
    *((int *) CMSG_DATA(cmhp)) = fd;

    /*
       We could rewrite the preceding lines as:

       control_un.cmh.cmsg_len = CMSG_LEN(sizeof(int));
       control_un.cmh.cmsg_level = SOL_SOCKET;
       control_un.cmh.cmsg_type = SCM_RIGHTS;
     *((int *) CMSG_DATA(CMSG_FIRSTHDR(&msgh))) = fd;
     */

    ns = sendmsg(socket_fd, &msgh, 0);
    if (ns == -1)
        errExit("sendmsg");

    return EXIT_SUCCESS;
}

int recv_fd(int socket_fd, int* fd) {
    struct msghdr msgh;
    struct iovec iov;
    int data;
    ssize_t nr;

    /* Allocate a char array of suitable size to hold the ancillary data.
       However, since this buffer is in reality a 'struct cmsghdr', use a
       union to ensure that it is aligned as required for that structure. */
    union {
        struct cmsghdr cmh;
        char   control[CMSG_SPACE(sizeof(int))];
        /* Space large enough to hold an 'int' */
    } control_un;
    struct cmsghdr *cmhp;

    /* Set 'control_un' to describe ancillary data that we want to receive */

    control_un.cmh.cmsg_len = CMSG_LEN(sizeof(int));
    control_un.cmh.cmsg_level = SOL_SOCKET;
    control_un.cmh.cmsg_type = SCM_RIGHTS;

    /* Set 'msgh' fields to describe 'control_un' */

    msgh.msg_control = control_un.control;
    msgh.msg_controllen = sizeof(control_un.control);

    /* Set fields of 'msgh' to point to buffer used to receive (real)
       data read by recvmsg() */

    msgh.msg_iov = &iov;
    msgh.msg_iovlen = 1;
    iov.iov_base = &data;
    iov.iov_len = sizeof(int);

    msgh.msg_name = NULL;               /* We don't need address of peer */
    msgh.msg_namelen = 0;

    /* Receive real plus ancillary data */

    nr = recvmsg(socket_fd, &msgh, 0);
    if (nr == -1)
        errExit("recvmsg");

    /* Get the received file descriptor (which is typically a different
       file descriptor number than was used in the sending process) */

    cmhp = CMSG_FIRSTHDR(&msgh);
    if (cmhp == NULL || cmhp->cmsg_len != CMSG_LEN(sizeof(int)))
        fatal("bad cmsg header / message length");
    if (cmhp->cmsg_level != SOL_SOCKET)
        fatal("cmsg_level != SOL_SOCKET");
    if (cmhp->cmsg_type != SCM_RIGHTS)
        fatal("cmsg_type != SCM_RIGHTS");

    *fd = *((int *) CMSG_DATA(cmhp));

    return EXIT_SUCCESS;
}
