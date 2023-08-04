#ifndef SHADOW_STL_THREADS
#define SHADOW_STL_THREADS

// Only support Linux
#include <pthread.h>

#include "include/stl_config.h"

SHADOW_STL_BEGIN_NAMESPACE

// Class _Refcount_Base provides a type, _RC_t, a data member,
// _M_ref_count, and member functions _M_incr and _M_decr, which perform
// atomic preincrement/predecrement.  The constructor initializes 
// _M_ref_count.

class _Refcount_Base {
public:
    typedef size_t RC_t;
    volatile RC_t _M_ref_count;
    pthread_mutex_t _M_ref_count_lock;
    // Constructor
    _Refcount_Base(RC_t n) : _M_ref_count(n) {
        pthread_mutex_init(&_M_ref_count_lock, nullptr);
    }

    // Atomic increment/decrement
    void _M_incr() { 
        pthread_mutex_lock(&_M_ref_count_lock);
        ++_M_ref_count;
        pthread_mutex_unlock(&_M_ref_count_lock);
    }
    RC_t _M_decr() { 
        pthread_mutex_lock(&_M_ref_count_lock);
        volatile RC_t tmp = --_M_ref_count;
        pthread_mutex_unlock(&_M_ref_count_lock);
        return tmp;
    }
};

// Atomic swap on unsigned long
// This is guaranteed to behave as though it were atomic only if all
// possibly concurrent updates use _Atomic_swap.
// In some cases the operation is emulated with a lock.
template<int dummy>
struct _Swap_lock_struct {
    static pthread_mutex_t _S_swap_lock;
};

// TODO: why dummy
template<int dummy>
pthread_mutex_t _Swap_lock_struct<dummy>::_S_swap_lock = PTHREAD_MUTEX_INITIALIZER;

// This should be portable, but performance is expected
// to be quite awful.  This really needs platform specific
// code.
inline unsigned long _Atomic_swap(unsigned long *p, unsigned long q) {
    pthread_mutex_lock(&_Swap_lock_struct<0>::_S_swap_lock);
    unsigned long tmp = *p;
    *p = q;
    pthread_mutex_unlock(&_Swap_lock_struct<0>::_S_swap_lock);
    return tmp;
}

// Locking class.  Note that this class *does not have a constructor*.
// It must be initialized either statically, with __STL_MUTEX_INITIALIZER,
// or dynamically, by explicitly calling the _M_initialize member function.
// (This is similar to the ways that a pthreads mutex can be initialized.)
// There are explicit member functions for acquiring and releasing the lock.

// There is no constructor because static initialization is essential for
// some uses, and only a class aggregate (see section 8.5.1 of the C++
// standard) can be initialized that way.  That means we must have no
// constructors, no base classes, no virtual functions, and no private or
// protected members.

// Helper struct.  This is a workaround for various compilers that don't
// handle static variables in inline functions properly.
struct _Shadow_STL_mutex_lock {
    pthread_mutex_t _M_lock;

    _Shadow_STL_mutex_lock() { 
        pthread_mutex_init(&_M_lock, nullptr);
    }
    
    void _M_acquire_lock() { 
        pthread_mutex_lock(&_M_lock);
    }
    void _M_release_lock() { 
        pthread_mutex_unlock(&_M_lock);
    }
};

#define SHADOW_STL_MUTEX_INITIALIZER = { PTHREAD_MUTEX_INITIALIZER }

// A locking class that uses _STL_mutex_lock.  The constructor takes a
// reference to an _STL_mutex_lock, and acquires a lock.  The
// destructor releases the lock.  It's not clear that this is exactly
// the right functionality.  It will probably change in the future.
struct _Shadow_STL_auto_lock {
    _Shadow_STL_mutex_lock& _M_lock;

    // RAII 资源获取即初始化
    _Shadow_STL_auto_lock(_Shadow_STL_mutex_lock& _lock) : _M_lock(_lock) { 
        _M_lock._M_acquire_lock();
    }
    ~_Shadow_STL_auto_lock() { 
        _M_lock._M_release_lock();
    }
private:
    // Non-copyable
    void operator=(const _Shadow_STL_auto_lock&);
    _Shadow_STL_auto_lock(const _Shadow_STL_auto_lock&);
};

SHADOW_STL_END_NAMESPACE

#endif // SHADOW_STL_THREADS