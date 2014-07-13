#ifndef SYSTEM_EXCEPTION_H_INCLUDED
#define SYSTEM_EXCEPTION_H_INCLUDED

#include <exception>
#include <cstring>

namespace utils
{

class system_exception
    : public std::exception
{
public:
    explicit system_exception(int e)
        : errno_(e)
    { }
    /*virtual*/ ~system_exception() throw()
    { }

    /*virtual*/ char const * what() const throw()
    {
        return std::strerror(errno_);
    }

private:
    const int errno_;
};

}
#endif // SYSTEM_EXCEPTION_H_INCLUDED
