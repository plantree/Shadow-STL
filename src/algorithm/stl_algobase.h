#ifndef SHADOW_STL_INTERNAL_ALGOBASE_H
#define SHADOW_STL_INTERNAL_ALGOBASE_H

#ifndef SHADOW_STL_CONFIG_H
#include "include/stl_config.h"
#endif // SHADOW_STL_CONFIG_H

#ifndef SHADOW_STL_INTERNAL_PAIR_H
#include "container/stl_pair.h"
#endif // SHADOW_STL_INTERNAL_PAIR_H

#ifndef SHADOW_TYPE_TRAITS_H
#include "include/type_traits.h"
#endif // SHADOW_TYPE_TRAITS_H

#ifndef SHADOW_STL_INTERNAL_ITERATOR_H
#include "iterator/stl_iterator_base.h"
#endif // SHADOW_STL_INTERNAL_ITERATOR_H

#include <cstring>
#include <climits>
#include <cstddef>
#include <cstdlib>
#include <new>

SHADOW_STL_BEGIN_NAMESPACE

//--------------------------------------------------
// swap an iter_swap
template <typename ForwardIter1, typename ForwardIter2, typename T>
inline void _iter_swap(ForwardIter1 a, ForwardIter2 b, T*) {
    T tmp = *a;
    *a = *b;
    *b = tmp;
}

template <typename ForwardIter1, typename ForwardIter2>
inline void iter_swap(ForwardIter1 a, ForwardIter2 b) {
    _iter_swap(a, b, value_type(a));
}

template <typename T>
inline void swap(T& a, T& b) {
    T tmp = a;
    a = b;
    b = tmp;
}

//--------------------------------------------------
// min and max
template <typename T>
inline const T& min(const T& a, const T& b) {
    return b < a ? b : a;
}

template <typename T>
inline const T& max(const T& a, const T& b) {
    return a < b ? b : a;
}

//--------------------------------------------------
// using functor Compare
template <typename T, typename Compare>
inline const T& min(const T& a, const T& b, Compare comp) {
    return comp(b, a) ? b : a;
}

template <typename T, typename Compare>
inline const T& max(const T& a, const T& b, Compare comp) {
    return comp(a, b) ? b : a;
}

//--------------------------------------------------
// copy

// All of these auxiliary functions serve two purposes.  (1) Replace
// calls to copy with memmove whenever possible.  (Memmove, not memcpy,
// because the input and output ranges are permitted to overlap.)
// (2) If we're using random access iterators, then write the loop as
// a for loop with an explicit count.
template <typename InputIter, typename OutputIter, typename Distance>
inline OutputIter _copy(InputIter first, InputIter last, OutputIter result, input_iterator_tag, Distance*) {
    for (; first != last; ++first, ++result) {
        *result = *first;
    }
    return result;
}

template <typename RandomAccessIter, typename OutputIter, typename Distance>
inline OutputIter _copy(RandomAccessIter first, RandomAccessIter last, OutputIter result, random_access_iterator_tag, Distance*) {
    for (Distance n = last - first; n > 0; --n, ++first, ++result) {
        *result = *first;
    }
    return result;
}

// memmove
template <typename T>
inline T*
_copy_trivial(const T* first, const T* last, T* result) {
    memmove(result, first, sizeof(T) * (last - first));
    return result + (last - first);
}

template <typename InputIter, typename OutputIter>
inline OutputIter _copy_aux2(InputIter first, InputIter last, OutputIter result, _false_type) {
    return _copy(first, last, result, iterator_category(first), distance_type(first));
}

template <typename InputIter, typename OutputIter>
inline OutputIter _copy_aux2(InputIter first, InputIter last, OutputIter result, _true_type) {
    return _copy_trivial(first, last, result);
}

template <typename T>
inline T* _copy_aux2(const T* first, const T* last, T* result, _true_type) {
    return _copy_trivial(first, last, result);
}

template <typename InputIter, typename OutputIter, typename T>
inline OutputIter _copy_aux(InputIter first, InputIter last, OutputIter result, _false_type) {
    using Trival = typename _type_traits<T>::has_trivial_assignment_operator;
    return _copy_aux2(first, last, result, Trival());
}

template <typename InputIter, typename OutputIter>
inline OutputIter copy(InputIter first, InputIter last, OutputIter result, _true_type) {
    return _copy_aux(first, last, result, value_type(first));
}

template <typename InputIter, typename OutputIter>
static OutputIter copy(InputIter first, InputIter last, OutputIter result) {
    using Category = typename iterator_traits<InputIter>::iterator_category;
    using Distance = typename iterator_traits<InputIter>::difference_type;
    return _copy(first, last, result, Category(), static_cast<Distance*>(0));
}

template <typename T>
static T* copy(const T* first, const T* last, T* result) {
    return _copy_trivial(first, last, result);
}

//--------------------------------------------------
// copy_backward
template <typename BidirectionalIter1, typename BidirectionalIter2, typename Distance>
inline BidirectionalIter2 _copy_backward(BidirectionalIter1 first, BidirectionalIter1 last, BidirectionalIter2 result, bidirectional_iterator_tag, Distance*) {
    while (first != last) {
        *--result = *--last;
    }
    return result;
}

template <typename RandomAccessIter, typename BidirectionalIter, typename Distance>
inline BidirectionalIter _copy_backward(RandomAccessIter first, RandomAccessIter last, BidirectionalIter result, random_access_iterator_tag, Distance*) {
    for (Distance n = last - first; n > 0; --n) {
        *--result = *--last;
    }
    return result;
}

template <typename BidirectionalIter1, typename BidirectionalIter2>
inline BidirectionalIter2 copy_backward(BidirectionalIter1 first, BidirectionalIter1 last, BidirectionalIter2 result) {
    return _copy_backward(first, last, result, iterator_category(first), distance_type(first));
}

//--------------------------------------------------
// copy_n (not part of the C++ standard)
template <typename InputIter, typename Size, typename OutputIter>
inline pair<InputIter, OutputIter> _copy_n(InputIter first, Size count, OutputIter result, input_iterator_tag) {
    for (; count > 0; --count, ++first, ++result) {
        *result = *first;
    }
    return pair<InputIter, OutputIter>(first, result);
}

template <typename RandomAccessIter, typename Size, typename OutputIter>
inline pair<RandomAccessIter, OutputIter> _copy_n(RandomAccessIter first, Size count, OutputIter result, random_access_iterator_tag) {
    RandomAccessIter last = first + count;
    return pair<RandomAccessIter, OutputIter>(last, copy(first, last, result));
}

template <typename InputIter, typename Size, typename OutputIter>
inline pair<InputIter, OutputIter> copy_n(InputIter first, Size count, OutputIter result) {
    return _copy_n(first, count, result, iterator_category(first));
}

//--------------------------------------------------
// fill and fill_n
template <typename ForwardIter, typename T>
void fill(ForwardIter first, ForwardIter last, const T& value) {
    for (; first != last; ++first) {
        *first = value;
    }
}

template <typename OutputIter, typename Size, typename T>
OutputIter fill_n(OutputIter first, Size count, const T& value) {
    for (; count > 0; --count, ++first) {
        *first = value;
    }
    return first;
}

// Specialization for fill_n with char, signed char, unsigned char.
inline void fill(unsigned char* first, unsigned char* last, const unsigned char& c) {
    unsigned char tmp = c;
    memset(first, tmp, last - first);
}

inline void fill(signed char* first, signed char* last, const signed char& c) {
    signed char tmp = c;
    memset(first, static_cast<unsigned char>(tmp), last - first);
}

inline void fill(char* first, char* last, const char& c) {
    char tmp = c;
    memset(first, static_cast<unsigned char>(tmp), last - first);
}

template <typename Size>
inline unsigned char* fill_n(unsigned char* first, Size count, const unsigned char& c) {
    fill(first, first + count, c);
    return first + count;
}

template <typename Size>
inline signed char* fill_n(signed char* first, Size count, const signed char& c) {
    fill(first, first + count, c);
    return first + count;
}

template <typename Size>
inline char* fill_n(char* first, Size count, const char& c) {
    fill(first, first + count, c);
    return first + count;
}

//--------------------------------------------------
// equal and mismatch
template <typename InputIter1, typename InputIter2>
pair<InputIter1, InputIter2> mismatch(InputIter1 first1, InputIter1 last1, InputIter2 first2) {
    while (first1 != last1 && *first1 == *first2) {
        ++first1;
        ++first2;
    }
    return pair<InputIter1, InputIter2>(first1, first2);
}

template <typename InputIter1, typename InputIter2, typename BinaryPredicate>
pair<InputIter1, InputIter2> mismatch(InputIter1 first1, InputIter1 last1, InputIter2 first2, BinaryPredicate binary_pred) {
    while (first1 != last1 && binary_pred(*first1, *first2)) {
        ++first1;
        ++first2;
    }
    return pair<InputIter1, InputIter2>(first1, first2);
}

template <typename InputIter1, typename InputIter2>
inline bool equal(InputIter1 first1, InputIter1 last1, InputIter2 first2) {
    for (; first1 != last1; ++first1, ++first2) {
        if (*first1 != *first2) {
            return false;
        }
    }
    return true;
}

template <typename InputIter1, typename InputIter2, typename BinaryPredicate>
inline bool equal(InputIter1 first1, InputIter1 last1, InputIter2 first2, BinaryPredicate binary_pred) {
    for (; first1 != last1; ++first1, ++first2) {
        if (!binary_pred(*first1, *first2)) {
            return false;
        }
    }
    return true;
}

//--------------------------------------------------
// lexicographical_compare and lexicographical_compare_3way.
// (the latter is not part of the C++ standard.)
template <typename InputIter1, typename InputIter2>
inline bool lexicographical_compare(InputIter1 first1, InputIter1 last1, InputIter2 first2, InputIter2 last2) {
    for (; first1 != last1 && first2 != last2; ++first1, ++first2) {
        if (*first1 < *first2) {
            return true;
        }
        if (*first2 < *first1) {
            return false;
        }
    }
    return first1 == last1 && first2 != last2;
}

template <typename InputIter1, typename InputIter2, typename Compare>
inline bool lexicographical_compare(InputIter1 first1, InputIter1 last1,
                                    InputIter2 first2, InputIter2 last2,
                                    Compare comp) {
    for (; first1 != last1 && first2 != last2; ++first1, ++first2) {
        if (comp(*first1, *first2)) {
            return true;
        } 
        if (comp(*first2, *first1)) {
            return false;
        }
    }
    return first1 == last1 && first2 != last2;
}

inline bool
lexicographical_compare(const unsigned char* first1,
                        const unsigned char* last1,
                        const unsigned char* first2,
                        const unsigned char* last2) {
    const size_t len1 = last1 - first1;
    const size_t len2 = last2 - first2;
    const int result = memcmp(first1, first2, min(len1, len2));
    return result != 0 ? result < 0 : len1 < len2;
}

inline bool
lexicographical_compare(const char* first1,
                        const char* last1,
                        const char* first2,
                        const char* last2) {
    return lexicographical_compare(
        reinterpret_cast<const unsigned char*>(first1),
        reinterpret_cast<const unsigned char*>(last1),
        reinterpret_cast<const unsigned char*>(first2),
        reinterpret_cast<const unsigned char*>(last2));
}

template <typename InputIter1, typename InputIter2>
inline int _lexicographical_compare_3way(InputIter1 first1, InputIter1 last1, InputIter2 first2, InputIter2 last2) {
    while (first1 != last1 && first2 != last2) {
        if (*first1 < *first2) {
            return -1;
        }
        if (*first2 < *first1) {
            return 1;
        }
        ++first1;
        ++first2;
    }
    if (first2 == last2) {
        return !(first1 == last1);
    } else {
        return -1;
    }
}

inline int _lexicographical_compare_3way(const unsigned char* first1, const unsigned char* last1, const unsigned char* first2, const unsigned char* last2) {
    const ptrdiff_t len1 = last1 - first1;
    const ptrdiff_t len2 = last2 - first2;
    const int result = memcmp(first1, first2, min(len1, len2));
    return result != 0 ? result : (len1 == len2 ? 0 : (len1 < len2 ? -1 : 1));
}

inline int _lexicographical_compare_3way(const char* first1, const char* last1, const char* first2, const char* last2) {
    return _lexicographical_compare_3way(reinterpret_cast<const unsigned char*>(first1), reinterpret_cast<const unsigned char*>(last1), reinterpret_cast<const unsigned char*>(first2), reinterpret_cast<const unsigned char*>(last2));
}

template <typename InputIter1, typename InputIter2>
int lexicographical_compare_3way(InputIter1 first1, InputIter1 last1, InputIter2 first2, InputIter2 last2) {
    return _lexicographical_compare_3way(first1, last1, first2, last2);
}


SHADOW_STL_END_NAMESPACE

#endif // SHADOW_STL_INTERNAL_ALGOBASE_H