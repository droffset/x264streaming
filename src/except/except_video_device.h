#ifndef EXCEPT_VIDEO_DEVICE_H_INCLUDED
#define EXCEPT_VIDEO_DEVICE_H_INCLUDED

#include <except/except_video_system.h>

class VideoDevice;

class ExceptVideoDevice
    : public ExceptVideoSystem
{
public:
    ExceptVideoDevice(int e, char const * func, VideoDevice * dev)
        : ExceptVideoSystem(e, func), dev_(dev)
    { }

    VideoDevice * device() const
    {
        return dev_;
    }

private:
    VideoDevice * dev_;
};

#endif // EXCEPT_VIDEO_DEVICE_H_INCLUDED
