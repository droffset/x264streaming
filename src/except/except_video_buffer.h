#ifndef EXCEPT_VIDEO_BUFFER_H_INCLUDED
#define EXCEPT_VIDEO_BUFFER_H_INCLUDED

#include <except/except_video_system.h>

class ExceptVideoBuffer
    : public ExceptVideoSystem
{
public:
    template <size_t N>
    ExceptVideoBuffer(int e, char const (&func)[N])
        : ExceptVideoSystem(e, func)
    { }
};

#endif // EXCEPT_VIDEO_BUFFER_H_INCLUDED
