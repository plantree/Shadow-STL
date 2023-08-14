#ifndef SHADOW_STL_INTERNAL_ITERATOR_BASE_H
#define SHADOW_STL_INTERNAL_ITERATOR_BASE_H

/* NOTE: This is an internal header file, included by other STL headers.
 *   You should not attempt to use it directly.
 */

// This file contains all of the general iterator-related utilities.
// The internal file stl_iterator.h contains predefined iterators, 
// such as front_insert_iterator and istream_iterator.

#include <cstddef>
#include "include/stl_config.h"

SHADOW_STL_BEGIN_NAMESPACE

struct input_iterator_tag {};
struct output_iterator_tag {};
struct forward_iterator_tag : public input_iterator_tag {};
struct bidirectional_iterator_tag : public forward_iterator_tag {};
struct random_access_iterator_tag : public bidirectional_iterator_tag {};

// The base classes input_iterator, output_iterator, forward_iterator,
// bidirectional_iterator, and random_access_iterator are not part of
// the C++ standard.  (They have been replaced by struct iterator.)
// They are included for backward compatibility with the HP STL.
template <typename T, typename Distance>
struct input_iterator {
    using iterator_category = input_iterator_tag;
    using value_type = T;
    using difference_type = Distance;
    using pointer = T*;
    using reference = T&;
};

struct output_iterator {
    using iterator_category = output_iterator_tag;
    using value_type = void;
    using difference_type = void;
    using pointer = void;
    using reference = void;
};

template <typename T, typename Distance>
struct forward_iterator {
    using iterator_category = forward_iterator_tag;
    using value_type = T;
    using difference_type = Distance;
    using pointer = T*;
    using reference = T&;
};

template <typename T, typename Distance>
struct bidirectional_iterator {
    using iterator_category = bidirectional_iterator_tag;
    using value_type = T;
    using difference_type = Distance;
    using pointer = T*;
    using reference = T&;
};

template <typename T, typename Distance>
struct random_access_iterator {
    using iterator_category = random_access_iterator_tag;
    using value_type = T;
    using difference_type = Distance;
    using pointer = T*;
    using reference = T&;
};

template <typename Category, typename T, typename Distance = std::ptrdiff_t, typename Pointer = T*, typename Reference = T&>
struct iterator {
    using iterator_category = Category;
    using value_type = T;
    using difference_type = Distance;
    using pointer = Pointer;
    using reference = Reference;
};

// iterator traits
template <typename T>
struct iterator_traits {
    using iterator_category = typename T::iterator_category;    // category
    using value_type = typename T::value_type;                  // value type
    using difference_type = typename T::difference_type;        // difference type
    using pointer = typename T::pointer;                        // pointer type
    using reference = typename T::reference;                    // reference type
};

template <typename T>
struct iterator_traits<T*> {
    using iterator_category = random_access_iterator_tag;
    using value_type = T;
    using difference_type = std::ptrdiff_t;
    using pointer = T*;
    using reference = T&;
};

template <typename T>
struct iterator_traits<const T*> {
    using iterator_category = random_access_iterator_tag;
    using value_type = T;
    using difference_type = std::ptrdiff_t;
    using pointer = const T*;
    using reference = const T&;
};

// The overloaded functions iterator_category, distance_type, and
// value_type are not part of the C++ standard.  (They have been
// replaced by struct iterator_traits.)  They are included for
// backward compatibility with the HP STL.

// We introduce internal names for these functions.
template <typename Iter>
inline typename iterator_traits<Iter>::iterator_category
_iterator_category(const Iter&) {
    using category = typename iterator_traits<Iter>::iterator_category;
    return category();
}

template <typename Iter>
inline typename iterator_traits<Iter>::difference_type*
_distance_type(const Iter&) {
    return static_cast<typename iterator_traits<Iter>::difference_type*>(0);
}

template <typename Iter>
inline 
typename iterator_traits<Iter>::value_type*
_value_type(const Iter&) {
    return static_cast<typename iterator_traits<Iter>::value_type*>(0);
}

template <typename Iter>
inline typename iterator_traits<Iter>::iterator_category
iterator_category(const Iter& it) {
    return _iterator_category(it);
}

template <typename Iter>
inline typename iterator_traits<Iter>::difference_type*
distance_type(const Iter& it) {
    return _distance_type(it);
}

template <typename Iter>
inline typename iterator_traits<Iter>::value_type*
value_type(const Iter& it) {
    return _value_type(it);
}

template <typename T, typename Distance>
inline input_iterator_tag
iterator_category(const input_iterator<T, Distance>&) {
    return input_iterator_tag();
}

inline output_iterator_tag
iterator_category(const output_iterator&) {
    return output_iterator_tag();
}

template <typename T, typename Distance>
inline forward_iterator_tag
iterator_category(const forward_iterator<T, Distance>&) {
    return forward_iterator_tag();
}

template <typename T, typename Distance>
inline bidirectional_iterator_tag
iterator_category(const bidirectional_iterator<T, Distance>&) {
    return bidirectional_iterator_tag();
}

template <typename T, typename Distance>
inline random_access_iterator_tag
iterator_category(const random_access_iterator<T, Distance>&) {
    return random_access_iterator_tag();
}

template <typename T>
inline random_access_iterator_tag
iterator_category(const T*) {
    return random_access_iterator_tag();
}

template <typename T, typename Distance>
inline T*
value_type(const input_iterator<T, Distance>&) {
    return static_cast<T*>(0);
}

template <typename T, typename Distance>
inline T*
value_type(const forward_iterator<T, Distance>&) {
    return static_cast<T*>(0);
}

template <typename T, typename Distance>
inline T*
value_type(const bidirectional_iterator<T, Distance>&) {
    return static_cast<T*>(0);
}

template <typename T, typename Distance>
inline T*
value_type(const random_access_iterator<T, Distance>&) {
    return static_cast<T*>(0);
}

template <typename T>
inline T*
value_type(const T*) {
    return static_cast<T*>(0);
}

template <typename T, typename Distance>
inline Distance*
distance_type(const input_iterator<T, Distance>&) {
    return static_cast<Distance*>(0);
}

template <typename T, typename Distance>
inline Distance*
distance_type(const forward_iterator<T, Distance>&) {
    return static_cast<Distance*>(0);
}

template <typename T, typename Distance>
inline Distance*
distance_type(const bidirectional_iterator<T, Distance>&) {
    return static_cast<Distance*>(0);
}

template <typename T, typename Distance>
inline Distance*
distance_type(const random_access_iterator<T, Distance>&) {
    return static_cast<Distance*>(0);
}

template <typename T>
inline std::ptrdiff_t*
distance_type(const T*) {
    return static_cast<std::ptrdiff_t*>(0);
}

// distance function
template <typename InputIterator, typename Distance>
inline void _distance(InputIterator first, InputIterator last, Distance& n, input_iterator_tag) {
    while (first != last) {
        ++first;
        ++n;
    }
}

template <typename RandomAccessIterator, typename Distance>
inline void _distance(RandomAccessIterator first, RandomAccessIterator last, Distance& n, random_access_iterator_tag) {
    n += last - first;
}

// random access iterator is a special case of input iterator
template <typename InputIterator, typename Distance>
inline void distance(InputIterator first, InputIterator last, Distance& n) {
    _distance(first, last, n, iterator_category(first));
}

template <typename InputIterator>
inline typename iterator_traits<InputIterator>::difference_type
_distance(InputIterator first, InputIterator last, input_iterator_tag) {
    typename iterator_traits<InputIterator>::difference_type n = 0;
    while (first != last) {
        ++first;
        ++n;
    }
    return n;
}

template <typename RandomAccessIterator>
inline typename iterator_traits<RandomAccessIterator>::difference_type
_distance(RandomAccessIterator first, RandomAccessIterator last, random_access_iterator_tag) {
    return last - first;
}

template <typename InputIterator>
inline typename iterator_traits<InputIterator>::difference_type
distance(InputIterator first, InputIterator last) {
    return _distance(first, last, iterator_category(first));
}

// advance function
template <typename InputIterator, typename Distance>
inline void _advance(InputIterator& it, Distance n, input_iterator_tag) {
    while (n--) {
        ++it;
    }
}

template <typename BidirectionalIterator, typename Distance>
inline void _advance(BidirectionalIterator& it, Distance n, bidirectional_iterator_tag) {
    if (n >= 0) {
        while (n--) {
            ++it;
        }
    } else {
        while (n++) {
            --it;
        }
    }
}

template <typename RandomAccessIterator, typename Distance>
inline void _advance(RandomAccessIterator& it, Distance n, random_access_iterator_tag) {
    it += n;
}

template <typename InputIterator, typename Distance>
inline void advance(InputIterator& it, Distance n) {
    _advance(it, n, iterator_category(it));
}

SHADOW_STL_END_NAMESPACE

#endif // SHADOW_STL_INTERNAL_ITERATOR_BASE_H