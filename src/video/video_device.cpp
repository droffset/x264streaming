#include <video/video_device.h>

#include <except/except_video_device.h>
#include <except/except_video_format.h>
#include <except/except_video_id.h>

#include <utils/xioctl.h>
#include <cstdio>
#include <climits>
#include <cassert>

#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <dirent.h>

#include <libv4l2.h>
//------------------------------------------------------------------------------
inline void VideoDevice::close() throw() // [private]
{
    ::v4l2_close(deviceFd_);
    deviceFd_ = -1;
    width_ = height_ = 0;
}

void VideoDevice::findDevicePath(utils::static_buffer<45> & path) //[private]
{
    path.append("/sys/bus/usb/devices/", sizeof("/sys/bus/usb/devices/") - 1)
        .append(deviceId_.c_str(), deviceId_.size())
        .append("/video4linux", sizeof("/video4linux") - 1);

    if(DIR * d = ::opendir(path.c_str()))
    {
        path.clear();
        for(struct dirent * cur = 0; (cur = ::readdir(d)); )
        {
            if(std::strncmp("video", cur->d_name, sizeof("video") - 1) == 0)
            {
                path.append("/dev/", sizeof("/dev/") - 1)
                    .append(cur->d_name);

                ::closedir(d);
                return;
            }
        }
        ::closedir(d);
        errno = 0;
    }
    throw ExceptVideoDevice(errno, "findDevicePath", this);
}
//------------------------------------------------------------------------------
VideoDevice::VideoDevice(char const * id, size_t num, bool active) throw()
    : deviceId_(id), deviceNum_(num)
    , active_(active)
    , deviceFd_(-1), width_(0), height_(0)
    , frame_(0)
    , buffer_()
{ }

VideoDevice::~VideoDevice() throw()
{
    if(isUp())
    {
        buffer_.releaseBuffers(this, std::nothrow);
        close();
    }
}
//------------------------------------------------------------------------------
void VideoDevice::streamOn()
{
    assert(isUp()); // never do this if device up

    ::v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if(!utils::xioctl(deviceFd_, VIDIOC_STREAMON, &type))
    {
        throw ExceptVideoDevice(errno, "VIDIOC_STREAMON", this);
    }
    fprintf(stdout, "<%s> STREAM ON\n", deviceId_.c_str());
}

void VideoDevice::streamOff()
{
    assert(isUp()); // never do this if device up

    ::v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if(!utils::xioctl(deviceFd_, VIDIOC_STREAMOFF, &type))
    {
        throw ExceptVideoDevice(errno, "VIDIOC_STREAMOFF", this);
    }
    fprintf(stdout, "<%s> STREAM OFF\n", deviceId_.c_str());
}
//------------------------------------------------------------------------------
void VideoDevice::activate()   // [async]
{
    active_ = true;
}

void VideoDevice::deactivate() // [async]
{
    active_ = false;
}

bool VideoDevice::isActive() const // [async]
{
    return active_;
}
//------------------------------------------------------------------------------
void VideoDevice::up(size_t width, size_t height, size_t numBuffs)
{
    {
        utils::static_buffer<45> devicePath;

        findDevicePath(devicePath);

        deviceFd_ = ::v4l2_open(devicePath.c_str(), O_RDWR | O_NONBLOCK, 0);
    }
    if(deviceFd_ < 0)
    {
        throw ExceptVideoDevice(errno, "v4l2_open", this);
    }
    ::v4l2_format fmt = {};

    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width       = width;
    fmt.fmt.pix.height      = height;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUV420;
    fmt.fmt.pix.field       = V4L2_FIELD_INTERLACED;

    if(!utils::xioctl(deviceFd_, VIDIOC_S_FMT, &fmt))
    {
        throw ExceptVideoDevice(errno, "VIDIOC_S_FMT", this);
    }
    if(fmt.fmt.pix.pixelformat != V4L2_PIX_FMT_YUV420)
    {
        throw ExceptVideoFormat();
    }
    if(fmt.fmt.pix.width != width || fmt.fmt.pix.height != height)
    {
        width  = fmt.fmt.pix.width;
        height = fmt.fmt.pix.height;

        fprintf(stdout, "Warning: driver is sending image at %dx%d\n", width, height);
    }
    width_  = width;
    height_ = height;

    buffer_.requestBuffers(this, numBuffs);

    fprintf(stdout, "Device <%s> is up!\n", deviceId_.c_str());
}

void VideoDevice::down() throw()
{
    buffer_.releaseBuffers(this, std::nothrow);
    close();

    fprintf(stdout, "Device <%s> is down!\n", deviceId_.c_str());
}
//------------------------------------------------------------------------------
