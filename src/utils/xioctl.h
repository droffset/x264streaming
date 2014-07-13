#ifndef XIOCTL_H_INCLUDED
#define XIOCTL_H_INCLUDED

#include <libv4l2.h>
#include <cerrno>

namespace utils
{

inline bool xioctl(int fd, int request, void * arg)
{
    int r;

    do
    {
        r = v4l2_ioctl(fd, request, arg);
    }
    while (r == -1 && ((errno == EINTR) || (errno == EAGAIN)));

    return r != -1;
}

}

#endif // XIOCTL_H_INCLUDED
