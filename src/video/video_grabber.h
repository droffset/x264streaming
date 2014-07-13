#ifndef VIDEO_GRABBER_H_INCLUDED
#define VIDEO_GRABBER_H_INCLUDED

#include <vector>

#include <utils/noncopyable.h>
#include <utils/event_observer.h>

class VideoSettings;
class VideoDevice;
class VideoX264Encoder;

class VideoGrabber
    : public utils::event_observer
{
public:
    explicit VideoGrabber(VideoSettings const & settings);
    ~VideoGrabber();

    void exec(VideoX264Encoder & encoder);

    size_t devices() const throw()
    {
        return devices_.size();
    }
// Shutdown grabber
    void shutdown() throw(); // [async]
// Restart all devices through its status
    void restart() throw();  // [async]

// Device is on until next restart, if it`s not activated
    void deviceOn(uint32_t num) throw();  // [async]
// Device is off until next restart, if it`s not deactivated
    void deviceOff(uint32_t num) throw(); // [async]

private:
    static bool checkId(char const * deviceId);

    void connect(size_t num);
    void disconnect(size_t num);

    void disconnectOnError(VideoDevice *) throw();

    typedef std::vector<VideoDevice *>::iterator dev_iterator;
    typedef std::vector<VideoDevice *>           dev_list;

    dev_list devices_;

    size_t const width_;
    size_t const height_;

    size_t const fps_;
    size_t const buffersCount_;
};

#endif // VIDEO_GRABBER_H_INCLUDED
