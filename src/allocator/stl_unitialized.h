#ifndef SHADOW_STL_INTERNAL_UNINITIALIZED_H
#define SHADOW_STL_INTERNAL_UNINITIALIZED_H

#include "include/stl_config.h"
#include "include/type_traits.h"
#include "allocator/stl_construct.h"
#include "container/stl_pair.h"
#include <cstring>

SHADOW_STL_BEGIN_NAMESPACE

// Valid if copy construction is equivalent to assignment, and if the
//  destructor is trivial.
template <typename InputIter, typename ForwardIter>
inline ForwardIter
_uninitialized_copy_aux(InputIter first, InputIter last, ForwardIter result, _true_type) {
    return copy(first, last, result);
}

template <typename InputIter, typename ForwardIter>
ForwardIter
_uninitialized_copy_aux(InputIter first, InputIter last, ForwardIter result, _false_type) {
    ForwardIter cur = result;
    for (; first != last; ++first, ++cur) {
        _Construct(&*cur, *first);
    }
    return cur;
}

template <typename InputIter, typename ForwardIter, typename T>
inline ForwardIter
_uninitialized_copy(InputIter first, InputIter last, ForwardIter result, T*) {
    using _Is_POD = typename _type_traits<T>::is_POD_type;
    return _uninitialized_copy_aux(first, last, result, _Is_POD());
}

template <typename InputIter, typename ForwardIter>
inline ForwardIter
uninitialized_copy(InputIter first, InputIter last, ForwardIter result) {
    return _uninitialized_copy(first, last, result, _value_type(result));
}

inline char* uninitialized_copy(const char* first, const char* last, char* result) {
    memmove(result, first, last - first);
    return result + (last - first);
}

inline wchar_t* 
uninitialized_copy(const wchar_t* first, const wchar_t* last, wchar_t* result) {
    memmove(result, first, sizeof(wchar_t) * (last - first));
    return result + (last - first);
}

// uninitialized_copy_n (not part of the C++ standard)
template <typename InputIter, typename Size, typename ForwardIter>
pair<InputIter, ForwardIter>
_uninitialized_copy_n(InputIter first, Size count, ForwardIter result, input_iterator_tag) {
    ForwardIter cur = result;
    for (; count > 0; --count, ++first, ++cur) {
        _Construct(&*cur, *first);
    }
    return pair<InputIter, ForwardIter>(first, cur);
}

template <typename RandomIter, typename Size, typename ForwardIter>
inline pair<RandomIter, ForwardIter>
_uninitialized_copy_n(RandomIter first, Size count, ForwardIter result, random_access_iterator_tag) {
    RandomIter last = first + count;
    return pair<RandomIter, ForwardIter>(last, uninitialized_copy(first, last, result));
}

template <typename InputIter, typename Size, typename ForwardIter>
inline pair<InputIter, ForwardIter>
_uninitialized_copy_n(InputIter first, Size count, ForwardIter result) {
    return _uninitialized_copy_n(first, count, result, iterator_category(first));
}

template <typename InputIter, typename Size, typename ForwardIter>
inline pair<InputIter, ForwardIter>
uninitialized_copy_n(InputIter first, Size count, ForwardIter result) {
    return _uninitialized_copy_n(first, count, result);
}

// Valid if copy construction is equivalent to assignment, and if the
// destructor is trivial.
template <typename ForwardIter, typename T>
inline void
_uninitialized_fill_aux(ForwardIter first, ForwardIter last, const T& x, _true_type) {
    fill(first, last, x);
}

template <typename ForwardIter, typename T>
void
_uninitialized_fill_aux(ForwardIter first, ForwardIter last, const T& x, _false_type) {
    ForwardIter cur = first;
    for (; cur != last; ++cur) {
        _Construct(&*cur, x);
    }
}


template <typename ForwardIter, typename T, typename T1>
inline void
_uninitialized_fill(ForwardIter first, ForwardIter last, const T& x, T1*) {
    using _Is_POD = typename _type_traits<T1>::is_POD_type;
    _uninitialized_fill_aux(first, last, x, _Is_POD());
}

template <typename ForwardIter, typename T>
inline void
uninitialized_fill(ForwardIter first, ForwardIter last, const T& x) {
    _uninitialized_fill(first, last, x, _value_type(first));
}

// Valid if copy construction is equivalent to assignment, and if the
//  destructor is trivial.
template <typename ForwardIter, typename Size, typename T>
inline ForwardIter
_uninitialized_fill_n_aux(ForwardIter first, Size n, const T& x, _true_type) {
    return fill_n(first, n, x);
}

template <typename ForwardIter, typename Size, typename T>
ForwardIter
_uninitialized_fill_n_aux(ForwardIter first, Size n, const T& x, _false_type) {
    ForwardIter cur = first;
    for (; n > 0; --n, ++cur) {
        _Construct(&*cur, x);
    }
    return cur;
}

template <typename ForwardIter, typename Size, typename T, typename T1>
inline ForwardIter
_uninitialized_fill_n(ForwardIter first, Size n, const T& x, T1*) {
    using _Is_POD = typename _type_traits<T1>::is_POD_type;
    return _uninitialized_fill_n_aux(first, n, x, _Is_POD());
}

template <typename ForwardIter, typename Size, typename T>
inline ForwardIter
uninitialized_fill_n(ForwardIter first, Size n, const T& x) {
    return _uninitialized_fill_n(first, n, x, _value_type(first));
}

// Extensions: __uninitialized_copy_copy, __uninitialized_copy_fill, 
// __uninitialized_fill_copy.

// __uninitialized_copy_copy
// Copies [first1, last1) into [result, result + (last1 - first1)), and
//  copies [first2, last2) into
//  [result, result + (last1 - first1) + (last2 - first2)).
template <typename InputIter1, typename InputIter2, typename ForwardIter>
inline ForwardIter
__uninitialized_copy_copy(InputIter1 first1, InputIter1 last1, InputIter2 first2, InputIter2 last2, ForwardIter result) {
    ForwardIter mid = uninitialized_copy(first1, last1, result);
    try {
        return uninitialized_copy(first2, last2, mid);
    }
    catch (...) {
        destroy(result, mid);
        throw;
    }
}

// __uninitialized_fill_copy
// Fills [result, mid) with x, and copies [first, last) into
//  [mid, mid + (last - first)).
template <typename ForwardIter, typename T, typename InputIter>
inline ForwardIter
__uninitialized_fill_copy(ForwardIter result, ForwardIter mid, const T& x, InputIter first, InputIter last) {
    uninitialized_fill(result, mid, x);
    try {
        return uninitialized_copy(first, last, mid);
    }
    catch (...) {
        destroy(result, mid);
        throw;
    }
}

// __uninitialized_copy_fill
// Copies [first1, last1) into [first2, first2 + (last1 - first1)), and
//  fills [first2 + (last1 - first1), last2) with x.
template <typename InputIter, typename ForwardIter, typename T>
inline void
__uninitialized_copy_fill(InputIter first1, InputIter last1, ForwardIter first2, ForwardIter last2, const T& x) {
    ForwardIter mid2 = uninitialized_copy(first1, last1, first2);
    try {
        uninitialized_fill(mid2, last2, x);
    }
    catch (...) {
        destroy(first2, mid2);
        throw;
    }
}

SHADOW_STL_END_NAMESPACE

#endif // SHADOW_STL_INTERNAL_UNINITIALIZED_H