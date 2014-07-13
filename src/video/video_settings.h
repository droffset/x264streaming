#ifndef VIDEO_SETTINGS_H_INCLUDED
#define VIDEO_SETTINGS_H_INCLUDED

#include <stdint.h>
#include <libconfig.h>

#include <utils/noncopyable.h>

class VideoSettings
    : utils::noncopyable
{
public:
    class Devices
    {
    public:
        explicit Devices(::config_setting_t * settings) throw();

        size_t count() const throw();

        bool active(size_t num) const throw();
        char const * id(size_t num) const throw();

    private:
        ::config_setting_t * devices_;
    };

    class Rtp
    {
    public:
        explicit Rtp(::config_setting_t * settings) throw();

        char const * ip() const throw();
        uint16_t port() const throw();
        uint16_t portBase() const throw();

        double   timestampUnit() const throw();
        uint32_t timestampInc() const throw();

        double bandwidth() const throw();

        size_t maxPacketSize() const throw();
        size_t sendBufferSize() const throw();

    private:
        ::config_setting_t * rtp_;
    };

public:
    VideoSettings() throw();
    ~VideoSettings() throw();

    void open(char const * path);

    size_t fps() const throw();
    size_t width() const throw();
    size_t height() const throw();
    size_t buffers() const throw();
    Devices devices() const throw();
    Rtp rtp() const throw();

private:
    ::config_t cfg_;
};

#endif // VIDEO_SETTINGS_H_INCLUDED
