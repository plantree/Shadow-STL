#ifndef SHADOW_STL_INTERNAL_RAW_STORAGE_ITER_H
#define SHADOW_STL_INTERNAL_RAW_STORAGE_ITER_H

#include "include/stl_config.h"
#include "iterator/stl_iterator_base.h"

SHADOW_STL_BEGIN_NAMESPACE

template <typename ForwardIter, typename T>
class raw_storage_iterator {
protected:
    ForwardIter _M_iter;
public:
    using iterator_category = output_iterator_tag;
    using value_type = void;
    using difference_type = void;
    using pointer = void;
    using reference = void;

    explicit raw_storage_iterator(ForwardIter x) : _M_iter(x) {}
    raw_storage_iterator& operator*() { return *this; }
    raw_storage_iterator& operator=(const T& element) {
        construct(&*_M_iter, element);
        return *this;
    }
    // ++a
    raw_storage_iterator<ForwardIter, T>& operator++() {
        ++_M_iter;
        return *this;
    }
    // a++
    raw_storage_iterator<ForwardIter, T> operator++(int) {
        raw_storage_iterator<ForwardIter, T> tmp = *this;
        ++_M_iter;
        return tmp;
    }
};

template <typename ForwardIter, typename T>
inline output_iterator_tag
iterator_category(const raw_storage_iterator<ForwardIter, T>&) {
    return output_iterator_tag();
}

SHADOW_STL_END_NAMESPACE

#endif // SHADOW_STL_INTERNAL_RAW_STORAGE_ITER_H