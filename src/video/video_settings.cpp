#include <video/video_settings.h>

#include <except/except_video_settings.h>
//------------------------------------------------------------------------------
VideoSettings::Devices::Devices(::config_setting_t * settings) throw()
    : devices_(settings)
{ }

size_t VideoSettings::Devices::count() const throw()
{
    return devices_ ? ::config_setting_length(devices_) : 0;
}

bool VideoSettings::Devices::active(size_t num) const throw()
{
    ::config_setting_t * device = ::config_setting_get_elem(devices_, num);

    int activeVal = true;
    ::config_setting_lookup_bool(device, "active", &activeVal);
    return activeVal;
}
char const * VideoSettings::Devices::id(size_t num) const throw()
{
    ::config_setting_t * device = ::config_setting_get_elem(devices_, num);

    char const * idVal = "";
    ::config_setting_lookup_string(device, "id", &idVal);
    return idVal;
}
//------------------------------------------------------------------------------
VideoSettings::Rtp::Rtp(::config_setting_t * settings) throw()
    : rtp_(settings)
{ }

char const * VideoSettings::Rtp::ip() const throw()
{
    char const * ipVal = "127.0.0.1";
    if(rtp_)
    {
        ::config_setting_lookup_string(rtp_, "ip", &ipVal);
    }
    return ipVal;
}
uint16_t VideoSettings::Rtp::port() const throw()
{
    int portVal = 5000;
    if(rtp_)
    {
        ::config_setting_lookup_int(rtp_, "port", &portVal);
    }
    return portVal;
}
uint16_t VideoSettings::Rtp::portBase() const throw()
{
    int portBaseVal = 1234;
    if(rtp_)
    {
        ::config_setting_lookup_int(rtp_, "port_base", &portBaseVal);
    }
    return portBaseVal;
}

double   VideoSettings::Rtp::timestampUnit() const throw()
{
    double timestampUnitVal = 1./9000.;
    if(rtp_)
    {
        ::config_setting_lookup_float(rtp_, "timestamp_unit", &timestampUnitVal);
    }
    return timestampUnitVal;
}

uint32_t VideoSettings::Rtp::timestampInc() const throw()
{
    int timestampIncVal = 3600;
    if(rtp_)
    {
        ::config_setting_lookup_int(rtp_, "timestamp_inc", &timestampIncVal);
    }
    return timestampIncVal;
}

double VideoSettings::Rtp::bandwidth() const throw()
{
    double bandwidthVal = 10000;
    if(rtp_)
    {
        ::config_setting_lookup_float(rtp_, "bandwidth", &bandwidthVal);
    }
    return bandwidthVal;
}

size_t VideoSettings::Rtp::maxPacketSize() const throw()
{
    int maxPacketSizeVal = 1400;
    if(rtp_)
    {
        ::config_setting_lookup_int(rtp_, "max_packet_size", &maxPacketSizeVal);
    }
    return maxPacketSizeVal;
}

size_t VideoSettings::Rtp::sendBufferSize() const throw()
{
    int sendBufferSizeVal = 10000;
    if(rtp_)
    {
        ::config_setting_lookup_int(rtp_, "send_buffer_size", &sendBufferSizeVal);
    }
    return sendBufferSizeVal;
}
//------------------------------------------------------------------------------
VideoSettings::VideoSettings() throw()
    : cfg_()
{
    ::config_init(&cfg_);
}
VideoSettings::~VideoSettings() throw()
{
    ::config_destroy(&cfg_);
}
//------------------------------------------------------------------------------
void VideoSettings::open(char const * path)
{
    if(!::config_read_file(&cfg_, path))
    {
        throw ExceptVideoSettings(&cfg_);
    }
}
//------------------------------------------------------------------------------
size_t VideoSettings::fps() const throw()
{
    int fpsVal = 25;
    ::config_lookup_int(&cfg_, "fps", &fpsVal);
    return fpsVal;
}
size_t VideoSettings::width() const throw()
{
    int widthVal = 640;
    ::config_lookup_int(&cfg_, "width", &widthVal);
    return widthVal;
}
size_t VideoSettings::height() const throw()
{
    int heightVal = 480;
    ::config_lookup_int(&cfg_, "height", &heightVal);
    return heightVal;
}
size_t VideoSettings::buffers() const throw()
{
    int buffersVal = 2;
    ::config_lookup_int(&cfg_, "buffers", &buffersVal);
    return buffersVal;
}

VideoSettings::Devices VideoSettings::devices() const throw()
{
    return Devices(::config_lookup(&cfg_, "devices"));
}
//------------------------------------------------------------------------------
VideoSettings::Rtp VideoSettings::rtp() const throw()
{
    return Rtp(::config_lookup(&cfg_, "rtp"));
}
//------------------------------------------------------------------------------
