#ifndef VIDEO_DEVICE_H_INCLUDED
#define VIDEO_DEVICE_H_INCLUDED

#include <video/video_buffer.h>

#include <utils/static_buffer.h>
#include <string>

class VideoDevice
{
public:
    VideoDevice(char const * id, size_t num, bool active = false) throw();
    ~VideoDevice() throw();

    VideoBuffer & buffers()
    {
        return buffer_;
    }
    VideoBuffer const & buffers() const
    {
        return buffer_;
    }

    void activate();       // [async]
    void deactivate();     // [async]
    bool isActive() const; // [async]

    void up(size_t width, size_t height, size_t numBuffs);
    void down() throw();

    void streamOn();
    void streamOff();

    bool isUp() const
    { return deviceFd_ >= 0; }

    int fd() const
    { return deviceFd_;  }
    std::string const & id() const
    { return deviceId_;  }
    size_t num() const
    { return deviceNum_; }

    size_t width() const
    { return width_;   }
    size_t height() const
    { return height_;  }

    size_t pts(size_t fps) const
    {
        return double(frame_) / double(fps) * 1000000;
    }
    void nextFrame()
    {
        ++frame_;
    }

private:
    std::string const deviceId_;
    size_t const      deviceNum_;

    bool   active_;

    int    deviceFd_;
    size_t width_;
    size_t height_;
    size_t frame_;

    VideoBuffer buffer_;

    void close() throw();
    void findDevicePath(utils::static_buffer<45> & path);
};

#endif // VIDEO_DEVICE_H_INCLUDED
