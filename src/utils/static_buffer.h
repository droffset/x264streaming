#ifndef STATIC_BUFFER_H_INCLUDED
#define STATIC_BUFFER_H_INCLUDED

#include <cstring>

namespace utils
{

template <size_t N>
class static_buffer
{
public:
    typedef char         value_type;
    typedef char &       reference;
    typedef char const & const_reference;
    typedef char *       iterator;
    typedef char const * const_iterator;

    static_buffer()
        : offset_(0)
    { }

    enum
    {
        max_size = N + 1
    };
    static_buffer & append(value_type const * src)
    {
        iterator dest = &buf_[offset_];
        if(dest < &buf_[N])
        {
            size_t const n = N - offset_;
            size_t i = 0;
            while(i++ < n && (*dest++ = *src++));
            if(i > n)
            {
                *dest = 0;
            }
            offset_ += i - 1;
        }
        return *this;
    }
    static_buffer & append(value_type const * src, size_t len)
    {
        iterator dest = &buf_[offset_];
        if(dest < &buf_[N])
        {
            size_t const n = len < (max_size - offset_) ? len : (N - offset_);
            std::memcpy(dest, src, n);
            dest[n] = 0;
            offset_ += n;
        }
        return *this;
    }

//    static_buffer & append(utils::static_string const & src)
//    {
//        return append(src, src.length());
//    }

    void clear()
    {
        offset_ = 0;
    }

    char const * c_str() const
    {
        return buf_;
    }
    size_t size() const
    {
        return offset_;
    }

    iterator begin()
    {
        return &buf_[0];
    }
    iterator end()
    {
        return &buf_[offset_];
    }

    const_iterator begin() const
    {
        return &buf_[0];
    }
    const_iterator end() const
    {
        return &buf_[offset_];
    }

    reference operator[](size_t idx)
    {
        return buf_[idx];
    }
    const_reference operator[](size_t idx) const
    {
        return buf_[idx];
    }

private:
    value_type buf_[max_size];
    size_t     offset_;

};

}

#endif // STATIC_BUFFER_H_INCLUDED
