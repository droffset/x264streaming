#ifndef EXCEPT_RTP_CHANNEL_H_INCLUDED
#define EXCEPT_RTP_CHANNEL_H_INCLUDED

#include <stdexcept>

class ExceptRtpChannel
    : public std::runtime_error
{
public:
    ExceptRtpChannel(std::string const & x)
        : std::runtime_error(x)
    {}
};


#endif // EXCEPT_RTP_CHANNEL_H_INCLUDED
