#ifndef RAII_CLEANUP_H_INCLUDED
#define RAII_CLEANUP_H_INCLUDED

#include <utils/noncopyable.h>

namespace utils
{

template <typename Owner, void (Owner::*Cleanup)()>
class raii_cleanup
    : utils::noncopyable
{
public:
    explicit raii_cleanup(Owner & owner)
        : owner_(owner), cleanup_(true)
    {}
    ~raii_cleanup()
    {
        if(cleanup_)
            (owner_.*Cleanup)();
    }

    void release()
    {
        cleanup_ = false;
    }

private:
    Owner & owner_;
    bool    cleanup_;
};

}

#endif // RAII_CLEANUP_H_INCLUDED
