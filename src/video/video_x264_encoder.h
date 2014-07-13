#ifndef VIDEO_X264_ENCODER_H_INCLUDED
#define VIDEO_X264_ENCODER_H_INCLUDED

#include <utils/event_handler.h>
#include <utils/noncopyable.h>
//------------------------------------------------------------------------------
#include <cstddef>
#include <stdint.h>
extern "C"
{
#include <x264.h>
}
//------------------------------------------------------------------------------
class  VideoDevice;
class  RtpChannel;

class VideoX264Encoder
    : utils::noncopyable
    , public utils::event_handler
{
public:
    enum { PAYLOAD_TYPE = 96 };

    explicit VideoX264Encoder(RtpChannel & dest) throw();
    ~VideoX264Encoder() throw();

    void init(size_t width, size_t height, size_t fps);

    uint32_t interrupt() const
    {
        return reason_;
    }
    uint32_t interruptArg() const
    {
        return arg_;
    }

protected:
    void data_in(void * data);
    void data_out(void * data);

    void hang_up(void * data);

    void error(void * devData, int error);

    void interrupt(uint64_t mes);

private:
    void x264PicInit(x264_picture_t &, VideoDevice *, uint8_t * data);

private:
    RtpChannel & channel_;

    size_t   fps_;
    x264_t * enc_;

    uint32_t reason_;
    uint32_t arg_;
};
//------------------------------------------------------------------------------
#endif // VIDEO_X264_ENCODER_H_INCLUDED
