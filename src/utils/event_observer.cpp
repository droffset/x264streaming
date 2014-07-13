#include <utils/event_observer.h>

#include <utils/event_handler.h>
#include <utils/system_exception.h>

#include <sys/eventfd.h>
#include <cerrno>
#include <cstdio>
#include <unistd.h>

//------------------------------------------------------------------------------
namespace utils
{

event_observer::event_observer(size_t maxFds)
    : eCache_(maxFds + 1)
{
    epollFd_ = ::epoll_create(maxFds + 1);
    if(epollFd_ == -1)
    {
        throw utils::system_exception(errno);
    }
    eventFd_ = ::eventfd(0, EFD_NONBLOCK);
    if(eventFd_ == -1)
    {
        ::close(epollFd_);
        throw utils::system_exception(errno);
    }

    ::epoll_event ev = {};
    ev.events   = EPOLLIN;
    ev.data.ptr = &eventFd_;
    ::epoll_ctl(epollFd_, EPOLL_CTL_ADD, eventFd_, &ev);
}
//------------------------------------------------------------------------------
event_observer::~event_observer()
{
    ::close(eventFd_);
    ::close(epollFd_);
}
//------------------------------------------------------------------------------
event_observer::wait_status event_observer::wait(event_handler & eh, int timeout)
{
    const int cnt = ::epoll_wait(epollFd_, &eCache_[0], eCache_.size(), timeout);
    if(cnt < 0)
    {
        throw utils::system_exception(errno);
    }
    wait_status status = WAIT_SUCCESS;
    for(size_t nFd = 0; nFd < size_t(cnt); ++nFd)
    {
        void * data           = eCache_[nFd].data.ptr;
        const uint32_t events = eCache_[nFd].events;
        if(data == &eventFd_)
        {
            if(events & (EPOLLIN))
            {
                uint64_t mes;
                size_t ret = ::read(eventFd_, &mes, sizeof(uint64_t));
                if(ret != sizeof(uint64_t))
                {
                    throw utils::system_exception(errno);
                }
                status = WAIT_INTERRUPT;
                eh.interrupt(mes);
            }
            else if(events & (EPOLLERR))
            {
                throw utils::system_exception(errno);
            }
        }
        else
        {
            if(events & (EPOLLERR))
            {
                eh.error(data, errno);
            }
            else
            {
                if(events & (EPOLLIN))
                {
                    eh.data_in(data);
                }
                if(events & (EPOLLOUT))
                {
                    eh.data_out(data);
                }
                if(events & (EPOLLRDHUP | EPOLLHUP))
                {
                    eh.hang_up(data);
                }
            }
        }
    }
    return status;
}
//------------------------------------------------------------------------------
bool event_observer::interrupt(uint64_t mes) const
{
    int wRet = ::write(eventFd_, &mes, sizeof(uint64_t));
    return wRet == sizeof(uint64_t);
}
//------------------------------------------------------------------------------
}
