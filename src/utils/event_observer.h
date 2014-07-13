#ifndef EVENT_OBSERVER_H_INCLUDED
#define EVENT_OBSERVER_H_INCLUDED

#include <sys/epoll.h>

#include <utils/pthread_mutex.h>
#include <utils/noncopyable.h>
#include <utils/pod_vector.h>

namespace utils
{

class event_handler;

class event_observer
    : utils::noncopyable
{
public:
    enum wait_status
    {
        WAIT_SUCCESS   = 0
      , WAIT_INTERRUPT = 1
    };

    explicit event_observer(size_t maxFds = 1);

    ~event_observer();

    template <uint32_t Event>
    bool add_event(int fd, void * data = 0) throw()
    {
        ::epoll_event ev = {};
        ev.events   = Event | EPOLLRDHUP;
        ev.data.ptr = data;
        return ::epoll_ctl(epollFd_, EPOLL_CTL_ADD, fd, &ev) != -1;
    }
    template <uint32_t Event>
    bool mod_event(int fd, void * data = 0) throw()
    {
        ::epoll_event ev = {};
        ev.events   = Event;
        ev.data.ptr = data;
        return ::epoll_ctl(epollFd_, EPOLL_CTL_MOD, fd, &ev) != -1;
    }
    bool del_event(int fd, void * data = 0) throw()
    {
        ::epoll_event ev = {};
        ev.events   = EPOLLRDHUP;
        ev.data.ptr = data;
        return ::epoll_ctl(epollFd_, EPOLL_CTL_DEL, fd, &ev) != -1;
    }

    wait_status wait(utils::event_handler & eh, int timeout = -1);

    bool interrupt(uint64_t mes = 0) const; // [async]

private:
    utils::pod_vector< ::epoll_event > eCache_;

    int epollFd_;
    int eventFd_;
};

}
#endif // EVENT_OBSERVER_H_INCLUDED
