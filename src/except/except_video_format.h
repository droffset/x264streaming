#ifndef EXCEPT_VIDEO_FORMAT_H_INCLUDED
#define EXCEPT_VIDEO_FORMAT_H_INCLUDED

#include <except/except_video_logic.h>

class ExceptVideoFormat
    : public ExceptVideoLogic
{
public:
    ExceptVideoFormat()
        : ExceptVideoLogic("Libv4l didn't accept YUV420 format. Can't proceed.")
    { }
};

#endif // EXCEPT_VIDEO_FORMAT_H_INCLUDED
