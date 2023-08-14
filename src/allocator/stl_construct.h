#ifndef SHADOW_STL_INTERNAL_CONSTRUCT_H
#define SHADOW_STL_INTERNAL_CONSTRUCT_H

#include <new>  // placement new

#include "include/type_traits.h"
#include "iterator/stl_iterator_base.h"

// construct and destroy.  These functions are not part of the C++ standard,
// and are provided for backward compatibility with the HP STL.  We also
// provide internal names _Construct and _Destroy that can be used within
// the library, so that standard-conforming pieces don't have to rely on
// non-standard extensions.

// Internal names

template <typename T1, typename T2>
inline void _Construct(T1* p, const T2& value) {
    new ((void*)p) T1(value);
}

template <typename T>
inline void _Construct(T* p) {
    new ((void*)p) T();
}

template <typename T>
inline void _Destroy(T* p) {
    p->~T();
}

template <typename ForwardIterator>
void 
_destroy_aux(ForwardIterator first, ForwardIterator last, _false_type) {
    for (; first < last; ++first) {
        destroy(&*first);
    }
}

template <typename ForwardIterator>
inline void
_destroy_aux(ForwardIterator first, ForwardIterator last, _true_type) {
    // Do nothing
}

template <typename ForwardIterator, typename T>
inline void 
_destroy(ForwardIterator first, ForwardIterator last, T*) {
    using _Trivial_destructor = typename _type_traits<T>::has_trivial_destructor;
    _destroy_aux(first, last, _Trivial_destructor());
}

template <typename ForwardIterator>
inline void _Destroy(ForwardIterator first, ForwardIterator last) {
    _destroy(first, last, _value_type(first));
}

inline void _Destroy(char*, char*) {}
inline void _Destroy(wchar_t*, wchar_t*) {}
inline void _Destroy(int*, int*) {}
inline void _Destroy(long*, long*) {}
inline void _Destroy(float*, float*) {}
inline void _Destroy(double*, double*) {}

// --------------------------------------------------
// Old names from the HP STL.
template <typename T1, typename T2>
inline void construct(T1* p, const T2& value) {
    _Construct(p, value);
}

template <typename T>
inline void construct(T* p) {
    _Construct(p);
}

template <typename T>
inline void destroy(T* p) {
    _Destroy(p);
}

template <typename ForwardIterator>
inline void destroy(ForwardIterator first, ForwardIterator last) {
    _Destroy(first, last);
}

#endif // SHADOW_STL_INTERNAL_CONSTRUCT_H