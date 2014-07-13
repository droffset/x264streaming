#ifndef NONCOPYABLE_H_INCLUDED
#define NONCOPYABLE_H_INCLUDED

namespace utils
{

class noncopyable
{
protected:
    noncopyable() {}
    ~noncopyable() {}

private: // emphasize the following members are private
    noncopyable(noncopyable const &);
    noncopyable & operator=(noncopyable const &);
};

}

#endif // NONCOPYABLE_H_INCLUDED
