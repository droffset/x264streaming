#include <video/video_grabber.h>

#include <video/video_device.h>
#include <video/video_settings.h>
#include <video/video_x264_encoder.h>

#include <utils/event_handler.h>

#include <except/except_video_device.h>
#include <except/except_video_logic.h>
//------------------------------------------------------------------------------
#include <cstdio>
#include <memory>
#include <cassert>
#include <signal.h>
//------------------------------------------------------------------------------
enum InterruptReason
{
    INTERRUPT_SHUTDOWN = 1
  , INTERRUPT_RESTART
  , INTERRUPT_DEVICE_ON
  , INTERRUPT_DEVICE_OFF
};
//------------------------------------------------------------------------------
inline uint64_t interruptData(InterruptReason reason, uint32_t arg = 0)
{
    union
    {
        uint32_t params[2];
        uint64_t data;

    } message = { { reason, arg } };

    return message.data;
}
//------------------------------------------------------------------------------
inline bool VideoGrabber::checkId(char const * deviceId) //[private, static]
{

    return true;
}
//------------------------------------------------------------------------------
VideoGrabber::VideoGrabber(VideoSettings const & settings)
    : utils::event_observer(settings.devices().count())
    , devices_()
    , width_(settings.width())
    , height_(settings.height())
    , fps_(settings.fps())
    , buffersCount_(settings.buffers())
{
    VideoSettings::Devices const & devSettings = settings.devices();

    devices_.reserve(devSettings.count());

    int num = 0;
    try
    {
        for(size_t i = 0, sz = devSettings.count(); i < sz; ++i)
        {
            char const * deviceId = devSettings.id(i);

            if(checkId(deviceId))
            {
                std::auto_ptr<VideoDevice> dev(
                        new VideoDevice(deviceId, num, devSettings.active(i))
                );

                devices_.push_back(dev.get());

                dev.release();
                ++num;
            }
        }
    }
    catch(...)
    {
        while(--num >= 0)
        {
            delete devices_[num];
        }
        throw;
    }
}

VideoGrabber::~VideoGrabber()
{
    for(int i = devices_.size() - 1; i >= 0; --i)
    {
        delete devices_[i];
    }
}
//------------------------------------------------------------------------------
void VideoGrabber::shutdown() throw() // [async]
{
    this->interrupt(interruptData(INTERRUPT_SHUTDOWN));
}
void VideoGrabber::restart() throw()  // [async]
{
    this->interrupt(interruptData(INTERRUPT_RESTART));
}

void VideoGrabber::deviceOn(uint32_t num) throw()  // [async]
{
    this->interrupt(interruptData(INTERRUPT_DEVICE_ON, num));
}
void VideoGrabber::deviceOff(uint32_t num) throw() // [async]
{
    this->interrupt(interruptData(INTERRUPT_DEVICE_OFF, num));
}
//------------------------------------------------------------------------------
void VideoGrabber::disconnectOnError(VideoDevice * dev) throw()
{
    if(dev->isUp())
    {
        this->del_event(dev->fd(), dev);

        dev->down();
    }
    dev->deactivate();
}
//------------------------------------------------------------------------------
void VideoGrabber::connect(size_t num)
{
    assert(num < devices_.size());

    VideoDevice * dev = devices_[num];

    if(dev->isActive() && !dev->isUp())
    {
        dev->up(width_, height_, buffersCount_);

        dev->streamOn();

        this->add_event<EPOLLIN>(dev->fd(), dev);
    }
}
void VideoGrabber::disconnect(size_t num)
{
    assert(num < devices_.size());

    VideoDevice * dev = devices_[num];

    if(dev->isActive() && dev->isUp())
    {
        this->del_event(dev->fd(), dev);

        dev->streamOff();

        dev->down();
    }
}

void VideoGrabber::exec(VideoX264Encoder & encoder)
{
    bool shutdown = false;

    // Disable signal interruption
    ::sigset_t ss;
    ::sigfillset(&ss);
    ::pthread_sigmask(SIG_SETMASK, &ss, 0);

    while(!shutdown)
    {
        try
        {
            encoder.init(width_, height_, fps_);

            for(size_t i = 0, sz = devices_.size(); i < sz; ++i)
            {
                connect(i);
            }

            for(bool proceed = true; proceed; )
            {
                wait_status status = this->wait(encoder);
                if(status == WAIT_INTERRUPT)
                {
                    switch(encoder.interrupt())
                    {
                    case INTERRUPT_SHUTDOWN:
                        shutdown = true;
                    case INTERRUPT_RESTART:
                        proceed  = false;
                        break;
                    case INTERRUPT_DEVICE_ON:
                        connect(encoder.interruptArg());
                        break;
                    case INTERRUPT_DEVICE_OFF:
                        disconnect(encoder.interruptArg());
                        break;
                    }
                }
            }

            for(int i = devices_.size() - 1; i >= 0; --i)
            {
                // streamOff call only in normal exiting
                disconnect(i);
            }
        }
        catch(ExceptVideoDevice const & x)
        {
            disconnectOnError(x.device());
            fprintf(stderr, "[ERROR]: Device [%s] event: \"%s\"\n"
                                        , x.device()->id().c_str(), x.what());

            //runMaintenance(x.device());
            continue;
        }
        catch(ExceptVideoLogic const & x)
        {
            disconnectOnError(x.device());
            fprintf(stderr, "[ERROR]: Device [%s] event: \"%s\"\n"
                                        , x.device()->id().c_str(), x.what());

            continue;
        }
        catch(std::exception const & x)
        {
            shutdown = true;

            for(int i = devices_.size() - 1; i >= 0; --i)
            {
                disconnectOnError(devices_[i]);
            }
            fprintf(stderr, "[ERROR]: generic event: \"%s\"\n", x.what());
        }
    }
}
