#ifndef EXCEPT_VIDEO_SYSTEM_H_INCLUDED
#define EXCEPT_VIDEO_SYSTEM_H_INCLUDED

#include <exception>
#include <cstring>

class ExceptVideoSystem
    : public std::exception
{
public:
    ExceptVideoSystem(int e, char const * func)
        : errno_(e), function_(func)
    { }

    char const * what() const throw()
    {
        return std::strerror(errno_);
    }
    char const * function() const throw()
    {
        return function_;
    }

private:
    int          errno_;
    char const * function_;
};

#endif // EXCEPT_VIDEO_SYSTEM_H_INCLUDED
