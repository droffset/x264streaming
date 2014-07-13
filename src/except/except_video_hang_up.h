#ifndef EXCEPT_VIDEO_HANG_UP_H_INCLUDED
#define EXCEPT_VIDEO_HANG_UP_H_INCLUDED

#include <except/except_video_device.h>

class ExceptVideoHangUp
    : public ExceptVideoDevice
{
public:
    explicit ExceptVideoHangUp(VideoDevice * dev)
        : ExceptVideoDevice(0, "", dev), mes_("Hanging up")
    { }

    char const * what() const throw()
    {
        return mes_;
    }

private:
    char const * mes_;
};

#endif // EXCEPT_VIDEO_HANG_UP_H_INCLUDED
