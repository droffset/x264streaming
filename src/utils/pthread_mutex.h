#ifndef PTHREAD_MUTEX_H_INCLUDED
#define PTHREAD_MUTEX_H_INCLUDED

#include <pthread.h>

#include <utils/system_exception.h>
#include <utils/noncopyable.h>

namespace utils
{

class pthread_mutex
    : utils::noncopyable
{
public:
    pthread_mutex()
    {
        if(int e = ::pthread_mutex_init(&mutex_, NULL))
        {
            throw utils::system_exception(e);
        }
    }
    ~pthread_mutex() throw()
    {
        ::pthread_mutex_destroy(&mutex_);
    }

    void lock()
    {
        if(int e = ::pthread_mutex_lock(&mutex_))
        {
            throw utils::system_exception(e);
        }
    }

    void unlock()
    {
        ::pthread_mutex_unlock(&mutex_);
    }

private:
    ::pthread_mutex_t mutex_;
};

}
#endif // PTHREAD_MUTEX_H_INCLUDED
