#ifndef VIDEO_BUFFER_H_INCLUDED
#define VIDEO_BUFFER_H_INCLUDED

#include <utils/noncopyable.h>

#include <stdint.h>
#include <cstddef>
#include <new>

#include <linux/videodev2.h>

class VideoDevice;

class VideoBuffer
    : utils::noncopyable
{
public:
    VideoBuffer() throw();
    ~VideoBuffer() throw();

    inline uint8_t const * operator[](size_t index) const throw()
    {
        return static_cast<uint8_t const *>(bufs_[index].data);
    }
    inline uint8_t * operator[](size_t index) throw()
    {
        return static_cast<uint8_t *>(bufs_[index].data);
    }

    class Data
    {
    public:
        friend class VideoBuffer;

        inline operator uint8_t *() throw()
        {
            return self_[buf_.index];
        }
        inline size_t size() const throw()
        {
            return buf_.bytesused;
        }

    private:
        Data(VideoBuffer & self, VideoDevice * device);

        VideoBuffer & self_;
        ::v4l2_buffer buf_;
    };

    void requestBuffers(VideoDevice * device, size_t count);

    void releaseBuffers(VideoDevice * device);
    void releaseBuffers();
    void releaseBuffers(VideoDevice * device, const std::nothrow_t) throw();

    Data dequeue(VideoDevice * device);
    void enqueue(Data & data, VideoDevice * device);
    void enqueue(Data & data, VideoDevice * device, const std::nothrow_t) throw();

private:
    struct MmapBuffer
    {
        void * data;
        size_t size;

        MmapBuffer(size_t index, VideoDevice * device);
        ~MmapBuffer() throw();
    };

    MmapBuffer * bufs_;
    size_t       count_;

    static MmapBuffer * construct_buffers(size_t count, VideoDevice * device);
    static void destroy_buffers(MmapBuffer * p, size_t count) throw();
};

#endif // VIDEO_BUFFER_H_INCLUDED
