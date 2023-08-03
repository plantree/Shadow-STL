#ifndef SHADOW_STL_THREADS
#define SHADOW_STL_THREADS

// Supported threading models are native SGI, pthreads, uithreads
// (similar to pthreads, but based on an earlier draft of the Posix
// threads standard), and Win32 threads.  Uithread support by Jochen
// Schlick, 1999.

// Only support Linux
#include <pthread.h>

#include "include/stl_config.h"

SHADOW_STL_BEGIN_NAMESPACE

// Class _Refcount_Base provides a type, _RC_t, a data member,
// _M_ref_count, and member functions _M_incr and _M_decr, which perform
// atomic preincrement/predecrement.  The constructor initializes 
// _M_ref_count.

class Refcount_Base {
public:
    typedef size_t RC_t;
    volatile RC_t M_ref_count;
    pthread_mutex_t M_ref_count_lock;
    // Constructor
    Refcount_Base(RC_t n) : M_ref_count(n) {
        pthread_mutex_init(&M_ref_count_lock, nullptr);
    }

    // Atomic increment/decrement
    void M_incr() { 
        pthread_mutex_lock(&M_ref_count_lock);
        ++M_ref_count;
        pthread_mutex_unlock(&M_ref_count_lock);
    }
    RC_t M_decr() { 
        pthread_mutex_lock(&M_ref_count_lock);
        volatile RC_t tmp = --M_ref_count;
        pthread_mutex_unlock(&M_ref_count_lock);
        return tmp;
    }
};

SHADOW_STL_END_NAMESPACE


#endif // SHADOW_STL_THREADS