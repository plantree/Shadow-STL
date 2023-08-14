#ifndef SHADPW_STL_INTERNAL_PAIR_H
#define SHADPW_STL_INTERNAL_PAIR_H

#include "include/stl_config.h"

SHADOW_STL_BEGIN_NAMESPACE

template <typename T1, typename T2>
struct pair {
    using first_type = T1;
    using second_type = T2;

    T1 first;
    T2 second;

    pair() : first(T1()), second(T2()) {}
    pair(const T1& a, const T2& b) : first(a), second(b) {}

    template <typename U1, typename U2>
    pair(const pair<U1, U2>& p) : first(p.first), second(p.second) {}
};

template <typename T1, typename T2>
inline bool operator==(const pair<T1, T2>& x, const pair<T1, T2>& y) {
    return x.first == y.first && x.second == y.second;
}

template <typename T1, typename T2>
inline bool operator<(const pair<T1, T2>& x, const pair<T1, T2>& y) {
    return x.first < y.first || (!(y.first < x.first) && x.second < y.second);
}

template <typename T1, typename T2>
inline bool operator!=(const pair<T1, T2>& x, const pair<T1, T2>& y) {
    return !(x == y);
}

template <typename T1, typename T2>
inline bool operator>(const pair<T1, T2>& x, const pair<T1, T2>& y) {
    return y < x;
}

template <typename T1, typename T2>
inline bool operator<=(const pair<T1, T2>& x, const pair<T1, T2>& y) {
    return !(y < x);
}

template <typename T1, typename T2>
inline bool operator>=(const pair<T1, T2>& x, const pair<T1, T2>& y) {
    return !(x < y);
}

template <typename T1, typename T2>
inline pair<T1, T2> make_pair(const T1& x, const T2& y) {
    return pair<T1, T2>(x, y);
}

SHADOW_STL_END_NAMESPACE

#endif // SHADPW_STL_INTERNAL_PAIR_H