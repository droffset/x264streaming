#ifndef EVENT_HANDLER_H_INCLUDED
#define EVENT_HANDLER_H_INCLUDED

#include <stdint.h>

namespace utils
{

class event_handler
{
public:
    virtual void data_in(void * data)  = 0;
    virtual void data_out(void * data) = 0;
    virtual void hang_up(void * data)  = 0;
    virtual void error(void * data, int error) = 0;

    virtual void interrupt(uint64_t mes) = 0;
};

}

#endif // EVENT_HANDLER_H_INCLUDED
