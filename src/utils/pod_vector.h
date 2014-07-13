#ifndef POD_VECTOR_H_INCLUDED
#define POD_VECTOR_H_INCLUDED

#include <stdexcept>
#include <algorithm>
#include <memory>

#include <utils/noncopyable.h>

namespace utils
{
//------------------------------------------------------------------------------
// it was designed with raw buffers in mind. it uses realloc to grow the
// buffer in place, rather than new[]/memcpy/delete[], making read/recv loops
// execute faster.
//------------------------------------------------------------------------------
template <typename T>
class pod_vector
    : utils::noncopyable
{
private:
    struct memento
    {
        memento()
            : p(0), n(0), a(0)
        {}

        T * p;
        size_t n, a;
    };
    memento m;

    static size_t const max = size_t(-1u) / sizeof(T);

    void do_realloc(size_t new_size)
    {
        if(new_size > max)
        {
            throw std::length_error("pod_vector::reserve");
        }
        T* t = (T*)::realloc(m.p, sizeof(T) * new_size);
        if(!t)
        {
            throw std::bad_alloc();
        }
        m.p = t;
        m.a = new_size;
    }

public:
    typedef T        value_type;
    typedef size_t   size_type;
    typedef T*       iterator;
    typedef T const* const_iterator;

    ~pod_vector()
    {
        ::free(m.p);
    }
    pod_vector()
    { }
    pod_vector(size_t n)
    {
        this->resize(n);
    }
    pod_vector(size_t n, T t)
    {
        this->resize(n);
        std::fill_n(m.p, m.n, t);
    }
    template <typename Iter>
    pod_vector(Iter f, Iter l)
    {
        this->append(f, l);
    }
    void copy_to(pod_vector * b)
    {
        b->assign(m.p, m.p + m.n);
    }
    static size_t max_size()
    {
        return max;
    }
    size_t size() const
    {
        return m.n;
    }
    bool empty() const
    {
        return 0 == m.n;
    }
    size_t capacity() const
    {
        return m.a;
    }
    void clear()
    {
        m.n = 0;
    }
    iterator begin()
    {
        return m.p;
    }
    iterator end()
    {
        return m.p + m.n;
    }
    const_iterator begin() const
    {
        return m.p;
    }
    const_iterator end() const
    {
        return m.p + m.n;
    }
    T& operator[](size_t i)
    {
        return m.p[i];
    }
    T const & operator[](size_t i) const
    {
        return m.p[i];
    }
    T& front()
    {
        return m.p[0];
    }
    T const& front() const
    {
        return m.p[0];
    }
    T& back()
    {
        return m.p[m.n - 1];
    }
    T const & back() const
    {
        return m.p[m.n - 1];
    }
    void reserve(size_t new_size)
    {
        if(new_size > m.a)
        {
            this->do_realloc(new_size);
        }
    }
    void resize(size_t new_size)
    {
        if(new_size > m.a)
        {
            this->do_realloc(new_size);
        }
        m.n = new_size;
    }
    void trim(size_t new_size)
    {
        if(new_size != m.a)
            this->do_realloc(new_size);
        m.n = new_size;
    }
    void push_back(T t)
    {
        if(m.a == m.n)
            this->reserve(m.n ? 2 * m.n : 2);
        m.p[m.n] = t;
        ++m.n;
    }
    void pop_back()
    {
        --m.n;
    }
    template<class Iter>
    void append(Iter f, Iter l)
    {
        size_t n = std::distance(f, l);
        this->resize(m.n + n);
        std::uninitialized_copy(f, l, m.p + m.n - n);
    }
    template<class Iter>
    void insert(iterator pos, Iter f, Iter l)
    {
        size_t i = pos - m.p;
        size_t n = std::distance(f, l);
        this->resize(m.n + n);
        pos = m.p + i;
        std::copy_backward(pos, m.p + m.n - n, m.p + m.n);
        std::uninitialized_copy(f, l, pos);
    }
    template<class Iter>
    void assign(Iter f, Iter l)
    {
        this->clear();
        this->insert(m.p, f, l);
    }
    iterator erase(iterator f, iterator l)
    {
        std::copy(l, m.p + m.n, f);
        m.n -= l - f;
        return f;
    }
    iterator erase(iterator f)
    {
        if(m.p + m.n != f)
            this->erase(f, f + 1);
        return f;
    }
    void swap(pod_vector& b)
    {
        std::swap(m, b.m);
    }
};
//------------------------------------------------------------------------------
template<class T>
inline void swap(pod_vector<T> & a, pod_vector<T> & b)
{
    a.swap(b);
}
//------------------------------------------------------------------------------

} // end of namespace utils

#endif // POD_VECTOR_H_INCLUDED
