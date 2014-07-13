#include <video/video_buffer.h>

#include <except/except_video_device.h>

#include <stdexcept> // std::bad_alloc
#include <cstdlib>
#include <cerrno>
#include <cassert>

#include <sys/mman.h>

#include <libv4l2.h>

#include <utils/xioctl.h>

#include <video/video_device.h>
//------------------------------------------------------------------------------
inline VideoBuffer::MmapBuffer::MmapBuffer(size_t index, VideoDevice * device)
    : data(0), size(0)
{
    v4l2_buffer buf = {};

    buf.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    buf.index  = index;

    if(!utils::xioctl(device->fd(), VIDIOC_QUERYBUF, &buf))
    {
        throw ExceptVideoDevice(errno, "VIDIOC_QUERYBUF", device);
    }

    size_t bufL = buf.length;
    void * bufD = v4l2_mmap(NULL, bufL
                                , PROT_READ | PROT_WRITE
                                , MAP_SHARED, device->fd(), buf.m.offset);
    if(bufD == MAP_FAILED)
    {
        throw ExceptVideoDevice(errno, "v4l2_mmap", device);
    }

    //if(!utils::xioctl(device->fd(), VIDIOC_QBUF, &buf))
    //{
    //    v4l2_munmap(bufD, bufL);
    //    throw ExceptVideoDevice(errno, "VIDIOC_QBUF", device);
    //}
    size = bufL;
    data = bufD;
}

inline VideoBuffer::MmapBuffer::~MmapBuffer() throw()
{
    // we check all in constructor, no need to check again - it always correct!
    v4l2_munmap(data, size);
}
//------------------------------------------------------------------------------
VideoBuffer::MmapBuffer * VideoBuffer::construct_buffers(size_t count, VideoDevice * device)
{
    MmapBuffer * p = static_cast<MmapBuffer *>(std::malloc(count * sizeof(MmapBuffer)));
    if(!p)
    {
        throw std::bad_alloc();
    }
    size_t i = 0;
    try
    {
        for(; i < count; ++i)
        {
            ::new(p + i) MmapBuffer(i, device);
        }
    }
    catch(...)
    {
        destroy_buffers(p, i);
        throw;
    }
    return p;
}

void VideoBuffer::destroy_buffers(MmapBuffer * p, size_t count) throw()
{
    //printf("~destroy_buffers: n:%d\n", n);

    int n = count;
    while(--n >= 0)
    {
        (p + n)->~MmapBuffer();
    }
    std::free(p);
}
//------------------------------------------------------------------------------
VideoBuffer::VideoBuffer() throw()
    : bufs_(0), count_(0)
{ }

VideoBuffer::~VideoBuffer() throw()
{
    destroy_buffers(bufs_, count_);
}
//------------------------------------------------------------------------------
void VideoBuffer::requestBuffers(VideoDevice * device, size_t count)
{
    assert(count_ == 0 && bufs_ == 0);

    ::v4l2_requestbuffers req = {};

    req.count  = count;
    req.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;

    if(!utils::xioctl(device->fd(), VIDIOC_REQBUFS, &req))
    {
        throw ExceptVideoDevice(errno, "VIDIOC_REQBUFS", device);
    }
    fprintf(stdout, "Real count of buffers: %d\n", req.count);

    bufs_  = construct_buffers(req.count, device);
    count_ = req.count;

    for(size_t i = 0; i < count_; ++i)
    {
        ::v4l2_buffer buf = {};
        buf.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index  = i;

        if(!utils::xioctl(device->fd(), VIDIOC_QBUF, &buf))
        {
            throw ExceptVideoDevice(errno, "VIDIOC_QBUF", device);
        }
    }
}

void VideoBuffer::releaseBuffers()
{
    destroy_buffers(bufs_, count_);

    bufs_  = 0;
    count_ = 0;
}

void VideoBuffer::releaseBuffers(VideoDevice * device)
{
    v4l2_requestbuffers req = {};

    req.count  = 0;
    req.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;

    const bool ok = utils::xioctl(device->fd(), VIDIOC_REQBUFS, &req);
    const int  e  = errno;

    releaseBuffers();

    if(!ok)
    {
        throw ExceptVideoDevice(e, "VIDIOC_REQBUFS", device);
    }
}

void VideoBuffer::releaseBuffers(VideoDevice * device, const std::nothrow_t) throw()
{
    v4l2_requestbuffers req = {};

    req.count  = 0;
    req.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;

    utils::xioctl(device->fd(), VIDIOC_REQBUFS, &req);

    releaseBuffers();
}
//------------------------------------------------------------------------------
VideoBuffer::Data::Data(VideoBuffer & self, VideoDevice * device)
    : self_(self), buf_()
{
    buf_.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf_.memory = V4L2_MEMORY_MMAP;

    if(!utils::xioctl(device->fd(), VIDIOC_DQBUF, &buf_))
    {
        throw ExceptVideoDevice(errno, "VIDIOC_DQBUF", device);
    }
}

VideoBuffer::Data VideoBuffer::dequeue(VideoDevice * device)
{
    return Data(*this, device);
}

void VideoBuffer::enqueue(Data & data, VideoDevice * device)
{
    if(!utils::xioctl(device->fd(), VIDIOC_QBUF, &data.buf_))
    {
        throw ExceptVideoDevice(errno, "VIDIOC_QBUF", device);
    }
}

void VideoBuffer::enqueue(Data & data, VideoDevice * device, const std::nothrow_t) throw()
{
    utils::xioctl(device->fd(), VIDIOC_QBUF, &data.buf_);
}
//------------------------------------------------------------------------------
