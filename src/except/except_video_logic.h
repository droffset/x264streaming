#ifndef EXCEPT_VIDEO_LOGIC_H_INCLUDED
#define EXCEPT_VIDEO_LOGIC_H_INCLUDED

#include <stdexcept>

class VideoDevice;

class ExceptVideoLogic
    : public std::logic_error
{
public:
    explicit ExceptVideoLogic(std::string const & error)
        : std::logic_error(error)
    { }

    template <size_t N>
    explicit ExceptVideoLogic(char const (&error)[N])
        : std::logic_error(error)
    { }

    VideoDevice * device() const
    {
        return dev_;
    }

private:
    VideoDevice * dev_;

};

#endif // EXCEPT_VIDEO_LOGIC_H_INCLUDED
