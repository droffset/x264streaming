#ifndef EXCEPT_VIDEO_ID_H_INCLUDED
#define EXCEPT_VIDEO_ID_H_INCLUDED

#include <except/except_video_logic.h>

class ExceptVideoId
    : public ExceptVideoLogic
{
public:
    ExceptVideoId()
        : ExceptVideoLogic("Can`t find path to video device")
    { }
};

#endif // EXCEPT_VIDEO_ID_H_INCLUDED
