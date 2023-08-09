#ifndef SHADOW_STL_INTERNAL_ALLOC_H
#define SHADOW_STL_INTERNAL_ALLOC_H

// This implements some standard node allocators.  These are
// NOT the same as the allocators in the C++ draft standard or in
// in the original STL.  They do not encapsulate different pointer
// types; indeed we assume that there is only one pointer type.
// The allocation primitives are intended to allocate individual objects,
// not larger arenas as with the original STL allocators.

#ifndef SHADOW_THROW_BAD_ALLOC
#if defined(SHADOW_STL_NO_BAD_ALLOC) || !defined(SHADOW_STL_USE_EXCEPTIONS)
#include <stdio.h>
#include <stdlib.h>
#define SHADOW_THROW_BAD_ALLOC fprintf(stderr, "out of memory\n"); exit(1)
#else /* Standard conforming out-of-memory handling */
#include <new>
#define SHADOW_THROW_BAD_ALLOC throw std::bad_alloc()
#endif 

#endif // SHADOW_STL_NO_BAD_ALLOC

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "include/stl_threads.h"

#define SHADOW_NODE_ALLOCATOR_THREADS true
#define SHADOW_NODE_ALLOCATOR_LOCK if (threads) \
    { _S_node_allocator_lock._M_acquire_lock(); }
#define SHADOW_NODE_ALLOCATOR_UNLOCK if (threads) \
    { _S_node_allocator_lock._M_release_lock(); }


SHADOW_STL_BEGIN_NAMESPACE

// SGI STL first level allocator
template <int __inst>
class _malloc_alloc_template {
private:
    // handle oom
    static void* _S_oom_malloc(size_t);
    static void* _S_oom_realloc(void*, size_t);

    static void (* __malloc_alloc_oom_handler)();

public:
    static void* allocate(size_t n) {
        void* result = malloc(n);
        // allocate failed
        if (result == nullptr) result = _S_oom_malloc(n);
        return result;
    }

    static void deallocate(void* p, size_t /* n */) {
        free(p);
    }

    static void* reallocate(void* p, size_t /* old_sz */, size_t new_sz) {
        void* result = realloc(p, new_sz);
        if (result == nullptr) result = _S_oom_realloc(p, new_sz);
        return result;
    }

    // set oom handler
    static void (*__set_malloc_handler(void (*f)()))() {
        void (*old)() = __malloc_alloc_oom_handler;
        __malloc_alloc_oom_handler = f;
        return old;
    }
};

// malloc_alloc out-of-memory handling
template <int __inst>
void (* _malloc_alloc_template<__inst>::__malloc_alloc_oom_handler)() = nullptr;

template <int __inst>
void* _malloc_alloc_template<__inst>::_S_oom_malloc(size_t n) {
    void (* my_malloc_handler)();
    void* result;

    for (;;) {
        my_malloc_handler = __malloc_alloc_oom_handler;
        if (my_malloc_handler == nullptr) { SHADOW_THROW_BAD_ALLOC; }
        // call oom handler, and try to free some memory
        (*my_malloc_handler)();
        // try to allocate again
        result = malloc(n);
        if (result) return result;
    }
}

template <int __inst>
void* _malloc_alloc_template<__inst>::_S_oom_realloc(void* p, size_t n) {
    void (* my_malloc_handler)();
    void* result;

    for (;;) {
        my_malloc_handler = __malloc_alloc_oom_handler;
        if (my_malloc_handler == nullptr) { SHADOW_THROW_BAD_ALLOC; }
        // call oom handler, and try to free some memory
        (*my_malloc_handler)();
        // try to allocate again
        result = realloc(p, n);
        if (result) return result;
    }
}

using malloc_alloc = _malloc_alloc_template<0>;

template <class _Tp, class _Alloc>
class simple_alloc {
public:
    static _Tp* allocate(size_t n) {
        return n == 0 ? nullptr : (_Tp*)_Alloc::allocate(n * sizeof(_Tp));
    }
    static _Tp* allocate(void) {
        return (_Tp*)_Alloc::allocate(sizeof(_Tp));
    }
    static void deallocate(_Tp* p, size_t n) {
        if (n != 0) _Alloc::deallocate(p, n * sizeof(_Tp));
    }
    static void deallocate(_Tp* p) {
        _Alloc::deallocate(p, sizeof(_Tp));
    }
};

// Allocator adaptor to check size arguments for debugging.
// Reports errors using assert.  Checking can be disabled with
// NDEBUG, but it's far better to just use the underlying allocator
// instead when no checking is desired.
// There is some evidence that this can confuse Purify.
template <class _Alloc>
class debug_alloc {
private:
    // Size of space used to store size.  Note 
    // that this must be large enough to preserve
    // alignment.
    enum { _S_extra = 8 };
                                   
                            
public:
    static void* allocate(size_t n) {
        char* result = (char*)_Alloc::allocate(n + (size_t)_S_extra);
        *(size_t*)result = n;
        return result + (size_t)_S_extra;
    }

    static void deallocate(void* p, size_t n) {
        char* real_p = (char*)p - (size_t)_S_extra;
        assert(*(size_t*)real_p == n);
        _Alloc::deallocate(real_p, n + (size_t)_S_extra);
    }

    static void* reallocate(void* p, size_t old, size_t new_sz) {
        char* real_p = (char*)p - (size_t)_S_extra;
        assert(*(size_t*)real_p == old);
        char* result = (char*)_Alloc::reallocate(real_p, 
                                        old + (size_t)_S_extra, 
                                        new_sz + (size_t)_S_extra);
        *(size_t*)result = new_sz;
        return result + (size_t)_S_extra;
    }
};

// Default node allocator.
// With a reasonable compiler, this should be roughly as fast as the
// original STL class-specific allocators, but with less fragmentation.
// Default_alloc_template parameters are experimental and MAY
// DISAPPEAR in the future.  Clients should just use alloc for now.
//
// Important implementation properties:
// 1. If the client request an object of size > _MAX_BYTES, the resulting
//    object will be obtained directly from malloc.
// 2. In all other cases, we allocate an object of size exactly
//    _S_round_up(requested_size).  Thus the client has enough size
//    information that we can return the object to the proper free list
//    without permanently losing part of the object.
//

// The first template parameter specifies whether more than one thread
// may use this allocator.  It is safe to allocate an object from
// one instance of a default_alloc and deallocate it with another
// one.  This effectively transfers its ownership to the second one.
// This may have undesirable effects on reference locality.
// The second parameter is unreferenced and serves only to allow the
// creation of multiple default_alloc instances.
// Node that containers built on different allocator instances have
// different types, limiting the utility of this approach.


// SGI STL second level allocator
template <bool threads, int inst>
class _default_alloc_template {
private:
    enum { _ALIGN = 8 };
    enum { _MAX_BYTES = 128 };
    enum { _NFREELISTS = _MAX_BYTES / _ALIGN };

    // round up to multiple of _ALIGN
    static size_t _S_round_up(size_t bytes) {
        return (((bytes) + (size_t)_ALIGN - 1) & ~((size_t)(_ALIGN) - 1));
    }

    // free list
    union _Obj {
        union _Obj* _M_free_list_link;
        char _M_client_data[1]; // The client sees this.
    };
    static _Obj* volatile _S_free_list[];

    // Get a reference to the free list for a given block size.
    static size_t _S_freelist_index(size_t bytes) {
        return (((bytes) + (size_t)_ALIGN - 1) / (size_t)_ALIGN - 1);
    }

    // Returns an object of size n, and optionally adds to size n free list.
    static void* _S_refill(size_t n);
    // Allocates a chunk for nobjs of size "size".  nobjs may be reduced
    // if it is inconvenient to allocate the requested number.
    static char* _S_chunk_alloc(size_t size, int& nobjs);

    // Chunk allocation state.
    static char* _S_start_free; // start of memory pool
    static char* _S_end_free;   // end of memory pool
    static size_t _S_heap_size;
    static _Shadow_STL_mutex_lock _S_node_allocator_lock;
    // It would be nice to use _STL_auto_lock here.  But we
    // don't need the NULL check.  And we do need a test whether
    // threads have actually been started.
    class _Lock;
    friend _Lock;
    class _Lock {
        public:
            _Lock() {
                SHADOW_NODE_ALLOCATOR_LOCK;
            }
            ~_Lock() {
                SHADOW_NODE_ALLOCATOR_UNLOCK;
            }
    };
public:
    static void* allocate(size_t n) {
        void* ret = nullptr;

        // If n > _MAX_BYTES, allocate directly from malloc.
        if (n > (size_t)_MAX_BYTES) {
            ret = malloc_alloc::allocate(n);
        } else {
            _Obj* volatile* my_free_list = _S_free_list + _S_freelist_index(n);
            // Acquire the lock here with a constructor call.
            // This ensures that it is released in exit or during stack
            // unwinding.
            _Lock lock_instance;
            // __restrict means that the pointer does not alias any other.
            _Obj* __restrict result = *my_free_list;
            // If the free list is full, create a new chunk.
            if (result == nullptr) {
                ret = _S_refill(_S_round_up(n));
            } else {
                *my_free_list = result->_M_free_list_link;
                ret = result;
            }
        }
        return ret;
    }

    static void deallocate(void* p, size_t n) {
        if (n > (size_t)_MAX_BYTES) {
            malloc_alloc::deallocate(p, n);
        } else {
            _Obj* volatile* my_free_list = _S_free_list + _S_freelist_index(n);
            _Obj* q = (_Obj*)p;
            _Lock lock_instance;
            // deallocate will put the block back to the free list.
            q->_M_free_list_link = *my_free_list;
            *my_free_list = q;
        }
    }
    static void* reallocate(void* p, size_t old_sz, size_t new_sz);
};

using alloc = _default_alloc_template<SHADOW_NODE_ALLOCATOR_THREADS, 0>;
using single_client_alloc = _default_alloc_template<false, 0>;

template <bool threads, int inst>
inline bool operator!=(const _default_alloc_template<threads, inst>&,
                    const _default_alloc_template<threads, inst>&) {
    return false;
}

/* We allocate memory in large chunks in order to avoid fragmenting     */
/* the malloc heap too much.                                            */
/* We assume that size is properly aligned.                             */
/* We hold the allocation lock.                                         */
template <bool threads, int inst>
char*
_default_alloc_template<threads, inst>::_S_chunk_alloc(size_t size, int& nobjs) {
    char* result;

    size_t total_bytes = size * nobjs;
    size_t bytes_left = _S_end_free - _S_start_free;

    if (bytes_left >= total_bytes) {    // total size is enough
        result = _S_start_free;
        _S_start_free += total_bytes;
        return result;
    } else if (bytes_left >= size) {    // not enough, and provide at least one block
        nobjs = (int)(bytes_left / size);
        total_bytes = size * nobjs;
        result = _S_start_free;
        _S_start_free += total_bytes;
        return result;
    } else {
        size_t bytes_to_get = 2 * total_bytes + _S_round_up(_S_heap_size >> 4);
        // Try to make use of the left-over piece.
        if (bytes_left > 0) {
            _Obj* volatile* my_free_list = _S_free_list + _S_freelist_index(bytes_left);
            ((_Obj*)_S_start_free)->_M_free_list_link = *my_free_list;
            *my_free_list = (_Obj*)_S_start_free;
        }
        _S_start_free = (char*)malloc(bytes_to_get);
        // malloc failed
        if (_S_start_free == nullptr) {
            size_t i;
            _Obj* volatile* my_free_list;
            _Obj* p;
            // Try to make do with what we have.  That can't
            // hurt.  We do not try smaller requests, since that tends
            // to result in disaster on multi-process machines.
            for (i = size; i <= (size_t)_MAX_BYTES; i += (size_t)_ALIGN) {
                my_free_list = _S_free_list + _S_freelist_index(i);
                p = *my_free_list;
                if (p != nullptr) {
                    *my_free_list = p->_M_free_list_link;
                    _S_start_free = (char*)p;
                    _S_end_free = _S_start_free + i;
                    // Recurse to use up the space.
                    return _S_chunk_alloc(size, nobjs);
                    // Any leftover piece will eventually make it to the
                    // right free list.
                }
            }
            _S_end_free = nullptr;
            // Use the first allocator.
            _S_start_free = (char*)malloc_alloc::allocate(bytes_to_get);
            // This should either throw an
            // exception or remedy the situation.  Thus we assume it
            // succeeded.
        }
        _S_heap_size += bytes_to_get;
        _S_end_free = _S_start_free + bytes_to_get;
        // Recurse to allocate the chunk.
        return _S_chunk_alloc(size, nobjs);
    }
}

/* Returns an object of size __n, and optionally adds to size __n free list.*/
/* We assume that __n is properly aligned.                                */
/* We hold the allocation lock.                                         */
template <bool threads, int inst>
void*
_default_alloc_template<threads, inst>::_S_refill(size_t n) {
    int nobjs = 20;
    char* chunk = _S_chunk_alloc(n, nobjs);
    _Obj* volatile* my_free_list;
    _Obj* result;
    _Obj* current_obj;
    _Obj* next_obj;
    int i;

    // If only one block, return it to caller.
    if (1 == nobjs) {
        return chunk;
    }
    
    my_free_list = _S_free_list + _S_freelist_index(n);
    // Build free list in chunk.
    result = (_Obj*)chunk;
    *my_free_list = next_obj = (_Obj*)(chunk + n);
    for (i = 1;; ++i) {
        current_obj = next_obj;
        next_obj = (_Obj*)((char*)next_obj + n);
        if (nobjs - 1 == i) {
            current_obj->_M_free_list_link = nullptr;
            break;
        } else {
            current_obj->_M_free_list_link = next_obj;
        }
    }
    return result;
}

template <bool threads, int inst>
void*
_default_alloc_template<threads, inst>::reallocate(void* p, size_t old_sz, size_t new_sz) {
    void* result;
    size_t copy_sz;

    if (old_sz > (size_t)_MAX_BYTES && new_sz > (size_t)_MAX_BYTES) {
        return realloc(p, new_sz);
    }
    if (_S_round_up(old_sz) == _S_round_up(new_sz)) {
        return p;
    }
    result = allocate(new_sz);
    copy_sz = new_sz > old_sz ? old_sz : new_sz;
    memcpy(result, p, copy_sz);
    deallocate(p, old_sz);
    return result;
}

#ifdef SHADOW_STL_THREADS
template <bool threads, int inst>
_Shadow_STL_mutex_lock _default_alloc_template<threads, inst>::_S_node_allocator_lock;
#endif

template <bool threads, int inst>
char* _default_alloc_template<threads, inst>::_S_start_free = nullptr;

template <bool threads, int inst>
char* _default_alloc_template<threads, inst>::_S_end_free = nullptr;

template <bool threads, int inst>
typename _default_alloc_template<threads, inst>::_Obj* volatile _default_alloc_template<threads, inst>::_S_free_list[_NFREELISTS] = {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr};

template <bool threads, int inst>
size_t _default_alloc_template<threads, inst>::_S_heap_size = 0;

template <typename T>
class allocator {
    using _Alloc = alloc;
public:
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;
    using value_type = T;

    template <typename T1> struct rebind {
        using other = allocator<T1>;
    };

    allocator() noexcept {}
    allocator(const allocator&) noexcept {}
    template <typename T1> allocator(const allocator<T1>&) noexcept {}
    ~allocator() noexcept {}

    pointer address(reference x) const noexcept {
        return &x;
    }
    const_pointer address(const_reference x) const noexcept {
        return &x;
    }

    // n is permitted to be 0.  The C++ standard says nothing about what
    // the return value is when n == 0.
    T* allocate(size_type n, const void* hint = 0) {
        return n != 0 ? static_cast<T*>(_Alloc::allocate(n * sizeof(T))): nullptr;
    }

    // p is not permitted to be a null pointer.
    void deallocate(pointer p, size_type n) {
        _Alloc::deallocate(p, n * sizeof(T));
    }

    size_type max_size() const noexcept {
        return size_t(-1) / sizeof(T);
    }
    
    // placement new
    void construct(pointer p, const T& val) {
        new (p) T(val);
    }
    void destroy(pointer p) {
        p->~T();
    }
};

template <>
class allocator<void> {
public:
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using pointer = void*;
    using const_pointer = const void*;
    using value_type = void;

    template <typename T1> struct rebind {
        using other = allocator<T1>;
    };
};

template <typename T1, typename T2>
inline bool operator==(const allocator<T1>&, const allocator<T2>&) {
    return true;
}

template <typename T1, typename T2>
inline bool operator!=(const allocator<T1>&, const allocator<T2>&) {
    return false;
}

// Allocator adaptor to turn an SGI-style allocator (e.g. alloc, malloc_alloc)
// into a standard-conforming allocator.   Note that this adaptor does
// *not* assume that all objects of the underlying alloc class are
// identical, nor does it assume that all of the underlying alloc's
// member functions are static member functions.  Note, also, that 
// __allocator<_Tp, alloc> is essentially the same thing as allocator<_Tp>.
template <typename T, typename _Alloc>
struct _allocator {
    _Alloc _underlying_alloc;

    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;
    using value_type = T;

    template <typename T1> struct rebind {
        using other = _allocator<T1, _Alloc>;
    };

    _allocator() noexcept {}
    _allocator(const _allocator& a) noexcept : _underlying_alloc(a._underlying_alloc) {}
    template <typename T1> _allocator(const _allocator<T1, _Alloc>& a) noexcept : _underlying_alloc(a._underlying_alloc) {}
    ~_allocator() noexcept {}

    pointer address(reference x) const noexcept {
        return &x;
    }
    const_pointer address(const_reference x) const noexcept {
        return &x;
    }

    T* allocate(size_type n, const void* hint = 0) {
        return n != 0 ? static_cast<T*>(_underlying_alloc.allocate(n * sizeof(T))): nullptr;
    }

    void deallocate(pointer p, size_type n) {
        _underlying_alloc.deallocate(p, n * sizeof(T));
    }

    size_type max_size() const noexcept {
        return size_t(-1) / sizeof(T);
    }

    void construct(pointer p, const T& val) {
        new (p) T(val);
    }
    void destroy(pointer p) {
        p->~T();
    }
};

template <typename _Alloc>
class _allocator<void, _Alloc> {
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using pointer = void*;
    using const_pointer = const void*;
    using value_type = void;

    template <typename T1> struct rebind {
        using other = _allocator<T1, _Alloc>;
    };
};

template <typename T, typename _Alloc>
inline bool operator==(const _allocator<T, _Alloc>&, const _allocator<T, _Alloc>&) {
    return true;
}

template <typename T, typename _Alloc>
inline bool operator!=(const _allocator<T, _Alloc>&, const _allocator<T, _Alloc>&) {
    return false;
}

// Another allocator adaptor: _Alloc_traits.  This serves two
// purposes.  First, make it possible to write containers that can use
// either SGI-style allocators or standard-conforming allocator.
// Second, provide a mechanism so that containers can query whether or
// not the allocator has distinct instances.  If not, the container
// can avoid wasting a word of memory to store an empty object.

// This adaptor uses partial specialization.  The general case of
// _Alloc_traits<_Tp, _Alloc> assumes that _Alloc is a
// standard-conforming allocator, possibly with non-equal instances
// and non-static members.  (It still behaves correctly even if _Alloc
// has static member and if all instances are equal.  Refinements
// affect performance, not correctness.)

// There are always two members: allocator_type, which is a standard-
// conforming allocator type for allocating objects of type _Tp, and
// _S_instanceless, a static const member of type bool.  If
// _S_instanceless is true, this means that there is no difference
// between any two instances of type allocator_type.  Furthermore, if
// _S_instanceless is true, then _Alloc_traits has one additional
// member: _Alloc_type.  This type encapsulates allocation and
// deallocation of objects of type _Tp through a static interface; it
// has two member functions, whose signatures are
//    static _Tp* allocate(size_t)
//    static void deallocate(_Tp*, size_t)

// The fully general version.
template <typename T, typename _Alloc>
struct _Alloc_traits {
    static const bool _S_instanceless = false;
    using allocator_type = typename _Alloc::template rebind<T>::other;
};

template <typename T, typename _Alloc>
const bool _Alloc_traits<T, _Alloc>::_S_instanceless;

// The version for the default allocator.
template <typename T1, typename T2>
struct _Alloc_traits<T1, allocator<T2>> {
    static const bool _S_instanceless = true;
    using _Alloc_type = simple_alloc<T1, alloc>;
    using allocator_type = allocator<T1>;
};

// Versions for the predefined SGI-style allocators.
template <typename T, int inst>
struct _Alloc_traits<T, _malloc_alloc_template<inst>> {
    static const bool _S_instanceless = true;
    using _Alloc_type = simple_alloc<T, _malloc_alloc_template<inst>>;
    using allocator_type = _malloc_alloc_template<inst>;
};

template <typename T, bool threads, int inst>
struct _Alloc_traits<T, _default_alloc_template<threads, inst>> {
    static const bool _S_instanceless = true;
    using _Alloc_type = simple_alloc<T, _default_alloc_template<threads, inst>>;
    using allocator_type = _default_alloc_template<threads, inst>;
};

template <typename T, typename _Alloc>
struct _Alloc_traits<T, debug_alloc<_Alloc>> {
    static const bool _S_instanceless = true;
    using _Alloc_type = simple_alloc<T, debug_alloc<_Alloc>>;
    using allocator_type = debug_alloc<_Alloc>;
};

// Versions for the _allocator adaptor used with the predefined
// SGI-style allocators.
template <typename T1, typename T2, int inst>
struct _Alloc_traits<T1, _allocator<T2, _malloc_alloc_template<inst>>> {
    static const bool _S_instanceless = true;
    using _Alloc_type = simple_alloc<T1, _malloc_alloc_template<inst>>;
    using allocator_type = _allocator<T2, _malloc_alloc_template<inst>>;
};

template <typename T1, typename T2, bool threads, int inst>
struct _Alloc_traits<T1, _allocator<T2, _default_alloc_template<threads, inst>>> {
    static const bool _S_instanceless = true;
    using _Alloc_type = simple_alloc<T1, _default_alloc_template<threads, inst>>;
    using allocator_type = _allocator<T2, _default_alloc_template<threads, inst>>;
};

template <typename T1, typename T2, typename _Alloc>
struct _Alloc_traits<T1, _allocator<T2, debug_alloc<_Alloc>>> {
    static const bool _S_instanceless = true;
    using _Alloc_type = simple_alloc<T1, debug_alloc<_Alloc>>;
    using allocator_type = _allocator<T2, debug_alloc<_Alloc>>;
};


SHADOW_STL_END_NAMESPACE

#endif // SHADOW_STL_INTERNAL_ALLOC_H