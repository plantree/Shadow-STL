#ifndef SHADOW_STL_INTERNAL_BVECTOR_H
#define SHADOW_STL_INTERNAL_BVECTOR_H

#include "allocator/stl_alloc.h"
#include "container/vector/stl_vector.h"
#include "algorithm/stl_algobase.h"
#include "stl_iterator_base.h"
#include "type_traits.h"
#include <climits>
#include <cstddef>

SHADOW_STL_BEGIN_NAMESPACE

struct _Bit_reference {
    unsigned int* _M_p;
    unsigned int _M_mask;
    _Bit_reference(unsigned int* x, unsigned int y) 
        : _M_p(x), _M_mask(y) {}
    _Bit_reference() : _M_p(nullptr), _M_mask(0) {}
    operator bool() const {
        return !(!(*_M_p & _M_mask));
    }
    _Bit_reference& operator=(bool x) {
        if (x) {
            *_M_p |= _M_mask;
        } else {
            *_M_p &= ~_M_mask;
        }
        return *this;
    }
    _Bit_reference& operator=(const _Bit_reference& x) {
        return *this = bool(x);
    }
    bool operator==(const _Bit_reference& x) const {
        return bool(*this) == bool(x);
    }
    bool operator<(const _Bit_reference& x) const {
        return !bool(*this) && bool(x);
    }
    void flip() {
        *_M_p ^= _M_mask;
    }
};

inline void swap(_Bit_reference x, _Bit_reference y) {
    bool tmp = x;
    x = y;
    y = tmp;
}

struct _Bit_iterator_base : public random_access_iterator<bool, ptrdiff_t> {
    unsigned int* _M_p;
    unsigned int _M_offset;

    _Bit_iterator_base(unsigned int* x, unsigned int y) 
        : _M_p(x), _M_offset(y) {}
    
    void _M_bump_up() {
        if (_M_offset++ == WORD_BIT - 1) {
            _M_offset = 0;
            ++_M_p;
        }
    }

    void _M_bump_down() {
        if (_M_offset-- == 0) {
            _M_offset = WORD_BIT - 1;
            --_M_p;
        }
    }

    void _M_incr(ptrdiff_t i) {
        difference_type n = i + _M_offset;
        _M_p += n / WORD_BIT;
        n = n % WORD_BIT;
        if (n < 0) {
            _M_offset = n + WORD_BIT;
            --_M_p;
        } else {
            _M_offset = n;
        }
    }

    bool operator==(const _Bit_iterator_base& x) const {
        return _M_p == x._M_p && _M_offset == x._M_offset;
    }

    bool operator<(const _Bit_iterator_base& x) const {
        return _M_p < x._M_p || (_M_p == x._M_p && _M_offset < x._M_offset);
    }

    bool operator!=(const _Bit_iterator_base& x) const {
        return !(*this == x);
    }

    bool operator>(const _Bit_iterator_base& x) const {
        return x < *this;
    }

    bool operator<=(const _Bit_iterator_base& x) const {
        return !(x < *this);
    }

    bool operator>=(const _Bit_iterator_base& x) const {
        return !(*this < x);
    }
};

inline ptrdiff_t
operator-(const _Bit_iterator_base& x, const _Bit_iterator_base& y) {
    return WORD_BIT * (x._M_p - y._M_p) + (x._M_offset - y._M_offset);
}

struct _Bit_iterator : public _Bit_iterator_base {
    using reference = _Bit_reference;
    using pointer = _Bit_reference*;
    using iterator = _Bit_iterator;

    _Bit_iterator() : _Bit_iterator_base(nullptr, 0) {}
    _Bit_iterator(unsigned int* x, unsigned int y) 
        : _Bit_iterator_base(x, y) {}
    
    reference operator*() const {
        return reference(_M_p, 1U << _M_offset);
    }
    iterator& operator++() {
        _M_bump_up();
        return *this;
    }
    iterator operator++(int) {
        iterator tmp = *this;
        _M_bump_up();
        return tmp;
    }
    iterator& operator--() {
        _M_bump_down();
        return *this;
    }
    iterator operator--(int) {
        iterator tmp = *this;
        _M_bump_down();
        return tmp;
    }
    iterator& operator+=(difference_type i) {
        _M_incr(i);
        return *this;
    }
    iterator& operator-=(difference_type i) {
        *this += -i;
        return *this;
    }
    iterator operator+(difference_type i) const {
        iterator tmp = *this;
        return tmp += i;
    }
    iterator operator-(difference_type i) const {
        iterator tmp = *this;
        return tmp -= i;
    }
    reference operator[](difference_type i) const {
        return *(*this + i);
    }
};

inline _Bit_iterator
operator+(ptrdiff_t n, const _Bit_iterator& x) {
    return x + n;
}

struct _Bit_const_iterator : public _Bit_iterator_base {
    using reference = bool;
    using const_reference = bool;
    using pointer = const bool*;
    using const_iterator = _Bit_const_iterator;

    _Bit_const_iterator() : _Bit_iterator_base(nullptr, 0) {}
    _Bit_const_iterator(unsigned int* x, unsigned int y) 
        : _Bit_iterator_base(x, y) {}
    _Bit_const_iterator(const _Bit_iterator& x)
        : _Bit_iterator_base(x._M_p, x._M_offset) {}
    
    const_reference operator*() const {
        return _Bit_reference(_M_p, 1U << _M_offset);
    }
    const_iterator& operator++() {
        _M_bump_up();
        return *this;
    }
    const_iterator operator++(int) {
        const_iterator tmp = *this;
        _M_bump_up();
        return tmp;
    }
    const_iterator& operator--() {
        _M_bump_down();
        return *this;
    }
    const_iterator operator--(int) {
        const_iterator tmp = *this;
        _M_bump_down();
        return tmp;
    }
    const_iterator& operator+=(difference_type i) {
        _M_incr(i);
        return *this;
    }
    const_iterator& operator-=(difference_type i) {
        *this += -i;
        return *this;
    }
    const_iterator operator+(difference_type i) const {
        const_iterator tmp = *this;
        return tmp += i;
    }
    const_iterator operator-(difference_type i) const {
        const_iterator tmp = *this;
        return tmp -= i;
    }
    const_reference operator[](difference_type i) const {
        return *(*this + i);
    }
};

inline _Bit_const_iterator 
operator+(ptrdiff_t n, const _Bit_const_iterator& x) {
    return x + n;
}

// Bit-vector base class, which encapsulates the difference between
// old SGI-style allocators and standard-conforming allocators.

// Base class for ordinary allocators.
template <typename Allocator, bool is_static>
class _Bvector_alloc_base {
public:
    using allocator_type = typename _Alloc_traits<bool, Allocator>::allocator_type;
    allocator_type get_allocator() const {
        return _M_data_allocator;
    }
    _Bvector_alloc_base(const allocator_type& a) 
        : _M_data_allocator(a), _M_start(), _M_finish(), _M_end_of_storage(nullptr) {}

protected:
    unsigned int* _M_bit_alloc(size_t n) {
        return _M_data_allocator.allocate((n + WORD_BIT - 1) / WORD_BIT);
    }
    void _M_deallocate() {
        if (_M_start._M_p) {
            _M_data_allocator.deallocate(_M_start._M_p, _M_end_of_storage - _M_start._M_p);
        }
    }
    typename _Alloc_traits<unsigned int, Allocator>::allocator_type _M_data_allocator;
    _Bit_iterator _M_start;
    _Bit_iterator _M_finish;
    unsigned int* _M_end_of_storage;
};

// Specialization for instanceless allocators.
template <typename Allocator>
class _Bvector_alloc_base<Allocator, true> {
public:
    using allocator_type = typename _Alloc_traits<bool, Allocator>::allocator_type;
    allocator_type get_allocator() const {
        return allocator_type();
    }
    _Bvector_alloc_base(const allocator_type&) 
        : _M_start(), _M_finish(), _M_end_of_storage(nullptr) {}

protected:
    using _Alloc_type = typename _Alloc_traits<unsigned int, Allocator>::allocator_type;

    unsigned int* _M_bit_alloc(size_t n) {
        return _Alloc_type::allocate((n + WORD_BIT - 1) / WORD_BIT);
    }
    void _M_deallocate() {
        if (_M_start._M_p) {
            _Alloc_type::deallocate(_M_start._M_p, _M_end_of_storage - _M_start._M_p);
        }
    }

    _Bit_iterator _M_start;
    _Bit_iterator _M_finish;
    unsigned int* _M_end_of_storage;
};

template <typename Allocator>
class _Bvector_base : public _Bvector_alloc_base<Allocator, _Alloc_traits<bool, Allocator>::_S_instanceless> {
    using _Base = _Bvector_alloc_base<Allocator, _Alloc_traits<bool, Allocator>::_S_instanceless>;
public:
    using allocator_type = typename _Base::allocator_type;

    _Bvector_base(const allocator_type& a) : _Base(a) {}
    ~_Bvector_base() {
        _Base::_M_deallocate();
    }
};

template <typename Alloc>
class vector<bool, Alloc> : public _Bvector_base<Alloc> {
public:
    using value_type = bool;
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using reference = _Bit_reference;
    using const_reference = bool;
    using pointer = _Bit_reference*;
    using const_pointer = const bool*;

    using iterator = _Bit_iterator;
    using const_iterator = _Bit_const_iterator;

    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    using reverse_iterator = std::reverse_iterator<iterator>;

    using allocator_type = typename _Bvector_base<Alloc>::allocator_type;
    allocator_type get_allocator() const {
        return _Bvector_base<Alloc>::get_allocator();
    }

protected:
    using _Bvector_base<Alloc>::_M_bit_alloc;
    using _Bvector_base<Alloc>::_M_deallocate;
    using _Bvector_base<Alloc>::_M_start;
    using _Bvector_base<Alloc>::_M_finish;
    using _Bvector_base<Alloc>::_M_end_of_storage;

    void _M_initialize(size_type n) {
        unsigned int* q = _M_bit_alloc(n);
        _M_end_of_storage = q + (n + WORD_BIT - 1) / WORD_BIT;
        _M_start = iterator(q, 0);
        _M_finish = _M_start + difference_type(n);
    }

    void _M_insert_aux(iterator position, bool x) {
        if (_M_finish._M_p != _M_end_of_storage) {
            copy_backward(position, _M_finish, _M_finish + 1);
            *position = x;
            ++_M_finish;
        } else {
            const size_type old_size = size();
            const size_type len = old_size != 0 ? 2 * old_size : 1;
            unsigned int* q = _M_bit_alloc(len);
            iterator i = copy(begin(), position, iterator(q, 0));
            *i++ = x;
            _M_finish = copy(position, end(), i);
            _M_deallocate();
            _M_end_of_storage = q + (len + WORD_BIT - 1) / WORD_BIT;
            _M_start = iterator(q, 0);
        }
    }

    template <typename InputIterator>
    void _M_initialize_range(InputIterator first, InputIterator last, input_iterator_tag) {
        _M_start = iterator();
        _M_finish = iterator();
        _M_end_of_storage = nullptr;
        for (; first != last; ++first) {
            push_back(*first);
        }
    }

    template <typename ForwardIterator>
    void _M_initialize_range(ForwardIterator first, ForwardIterator last, forward_iterator_tag) {
        size_type n = distance(first, last);
        _M_initialize(n);
        uninitialized_copy(first, last, _M_start);
    }

    template <typename InputIterator>
    void _M_insert_range(iterator pos, InputIterator first, InputIterator last, input_iterator_tag) {
        for (; first != last; ++first) {
            pos = insert(pos, *first);
            ++pos;
        }
    }

    template <typename ForwardIterator>
    void _M_insert_range(iterator pos, ForwardIterator first, ForwardIterator last, forward_iterator_tag) {
        if (first != last) {
            size_type n = distance(first, last);
            if (capacity() - size() >= n) {
                copy_backward(pos, end(), _M_finish + difference_type(n));
                copy(first, last, pos);
                _M_finish += difference_type(n);
            } else {
                const size_type old_size = size();
                const size_type len = old_size + max(old_size, n);
                unsigned int* q = _M_bit_alloc(len);
                iterator i = copy(begin(), pos, iterator(q, 0));
                i = copy(pos, end(), i);
                _M_deallocate();
                _M_end_of_storage = q + (len + WORD_BIT - 1) / WORD_BIT;
                _M_start = iterator(q, 0);
            }
        }
    }

public:
    iterator begin() {
        return _M_start;
    }
    const_iterator begin() const {
        return _M_start;
    }
    iterator end() {
        return _M_finish;
    }
    const_iterator end() const {
        return _M_finish;
    }

    reverse_iterator rbegin() {
        return reverse_iterator(end());
    }
    const_reverse_iterator rbegin() const {
        return const_reverse_iterator(end());
    }
    reverse_iterator rend() {
        return reverse_iterator(begin());
    }
    const_reverse_iterator rend() const {
        return const_reverse_iterator(begin());
    }

    size_type size() const {
        return size_type(end() - begin());
    }
    size_type max_size() const {
        return size_type(-1);
    }
    size_type capacity() const {
        return size_type(const_iterator(_M_end_of_storage, 0) - begin());
    }
    bool empty() const {
        return begin() == end();
    }

    reference operator[](size_type n) {
        return *(begin() + difference_type(n));
    }
    const_reference operator[](size_type n) const {
        return *(begin() + difference_type(n));
    }

    void _M_range_check(size_type n) const {
        if (n >= size()) {
            throw std::out_of_range("vector<bool> subscript");
        }
    }

    reference at(size_type n) {
        _M_range_check(n);
        return (*this)[n];
    }
    const_reference at(size_type n) const {
        _M_range_check(n);
        return (*this)[n];
    }

    explicit vector(const allocator_type& a = allocator_type()) 
        : _Bvector_base<Alloc>(a) {}
    
    vector(size_type n, bool value, const allocator_type& a = allocator_type()) 
        : _Bvector_base<Alloc>(a) {
        _M_initialize(n);
        fill(_M_start._M_p, _M_end_of_storage, value ? ~0 : 0);
    }

    explicit vector(size_type n) : _Bvector_base<Alloc>(allocator_type()) {
        _M_initialize(n);
        fill(_M_start._M_p, _M_end_of_storage, 0);
    }

    vector(const vector& x) : _Bvector_base<Alloc>(x.get_allocator()) {
        _M_initialize(x.size());
        copy(x.begin(), x.end(), _M_start);
    }

    // Check whether it's an integral type.  If so, it's not an iterator.
    template <typename Integer>
    void _M_initialize_dispatch(Integer n, Integer x, _true_type) {
        _M_initialize(n);
        fill(_M_start._M_p, _M_end_of_storage, x ? ~0 : 0);
    }

    template <typename InputIerator>
    void _M_initialize_dispatch(InputIerator first, InputIerator last, _false_type) {
        _M_initialize_range(first, last, iterator_category(first));
    }

    template <typename InputIterator>
    vector(InputIterator first, InputIterator last, const allocator_type& a = allocator_type()) 
        : _Bvector_base<Alloc>(a) {
        using is_integer = typename _Is_integer<InputIterator>::_Integral;
        _M_initialize_dispatch(first, last, is_integer());
    }

    ~vector() {}

    vector& operator=(const vector& x) {
        if (&x == this) {
            return *this;
        }
        if (x.size() > capacity()) {
            _M_deallocate();
            _M_initialize(x.size());
        }
        copy(x.begin(), x.end(), begin());
        _M_finish = begin() + difference_type(x.size());
        return *this;
    }


    // assign(), a generalized assignment member function.  Two
    // versions: one that takes a count, and one that takes a range.
    // The range version is a member template, so we dispatch on whether
    // or not the type is an integer.
    void _M_fill_assign(size_t n, bool x) {
        if (n > size()) {
            fill(_M_start._M_p, _M_end_of_storage, x ? ~0 : 0);
            insert(end(), n - size(), x);   
        } else {
            erase(begin() + n, end());
            fill(_M_start._M_p, _M_end_of_storage, x ? ~0 : 0);
        }
    }

    void assign(size_t n, bool x) {
        _M_fill_assign(n, x);
    }

    template <typename InputIterator>
    void assign(InputIterator first, InputIterator last) {
        using is_integer = typename _Is_integer<InputIterator>::_Integral;
        _M_assign_dispatch(first, last, is_integer());
    }

    template <typename Integer>
    void _M_assign_dispatch(Integer n, Integer val, _true_type) {
        _M_fill_assign((size_type) n, (bool) val);
    }

    template <typename InputIterator>
    void _M_assign_dispatch(InputIterator first, InputIterator last, _false_type) {
        _M_assign_aux(first, last, iterator_category(first));
    }

    template <typename InputIterator>
    void _M_assign_aux(InputIterator first, InputIterator last, input_iterator_tag) {
        iterator cur = begin();
        for (; first != last && cur != end(); ++cur, ++first) {
            *cur = *first;
        }
        if (first == last) {
            erase(cur, end());
        } else {
            insert(end(), first, last);
        }
    }

    template <typename ForwardIterator>
    void _M_assign_aux(ForwardIterator first, ForwardIterator last, forward_iterator_tag) {
        size_type len = distance(first, last);
        if (len < size()) {
            erase(copy(first, last, begin(), end()));
        } else {
            ForwardIterator mid = first;
            advance(mid, size());
            copy(first, mid, begin());
            insert(end(), mid, last);
        }
    }

    void reserve(size_type n) {
        if (capacity() < n) {
            unsigned int* q = _M_bit_alloc(n);
            _M_finish = copy(begin(), end(), iterator(q, 0));
            _M_deallocate();
            _M_start = iterator(q, 0);
            _M_end_of_storage = q + (n + WORD_BIT - 1) / WORD_BIT;
        }
    }

    reference front() {
        return *begin();
    }
    const_reference front() const {
        return *begin();
    }
    reference back() {
        return *(end() - 1);
    }
    const_reference back() const {
        return *(end() - 1);
    }
    void push_back(bool x) {
        if (_M_finish._M_p != _M_end_of_storage) {
            *_M_finish = x;
            ++_M_finish;
        } else {
            _M_insert_aux(end(), x);
        }
    }

    void swap(vector& x) {
        swap(_M_start, x._M_start);
        swap(_M_finish, x._M_finish);
        swap(_M_end_of_storage, x._M_end_of_storage);
    }

    iterator insert(iterator position, bool x = bool()) {
        difference_type n = position - begin();
        if (_M_finish._M_p != _M_end_of_storage && position == end()) {
            *_M_finish = x;
            ++_M_finish;
        } else {
            _M_insert_aux(position, x);
        }
        return begin() + n;
    }

    template <typename Integer>
    void _M_insert_dispatch(iterator pos, Integer n, Integer x, _true_type) {
        _M_fill_insert(pos, (size_type) n, (bool) x);
    }

    template <typename InputIterator>
    void _M_insert_dispatch(iterator pos, InputIterator first, InputIterator last, _false_type) {
        _M_insert_range(pos, first, last, iterator_category(first));
    }

    template <typename InputIterator>
    void insert(iterator pos, InputIterator first, InputIterator last) {
        using is_integer = typename _Is_integer<InputIterator>::_Integral;
        _M_insert_dispatch(pos, first, last, is_integer());
    }

    // void insert(iterator position, const_iterator first, const_iterator last) {
    //     if (first != last) {
    //         difference_type n = distance(first, last);
    //         if (capacity() - size() >= n) {
    //             copy_backward(position, end(), _M_finish + n);
    //             copy(first, last, position);
    //             _M_finish += n;
    //         } else {
    //             const size_type old_size = size();
    //             const size_type len = old_size + max(old_size, n);
    //             unsigned int* q = _M_bit_alloc(len);
    //             iterator i = copy(begin(), position, iterator(q, 0));
    //             i = copy(first, last, i);
    //             _M_finish = copy(position, end(), i);
    //             _M_deallocate();
    //             _M_end_of_storage = q + (len + WORD_BIT - 1) / WORD_BIT;
    //             _M_start = iterator(q, 0);
    //         }
    //     }
    // }

    // void insert(iterator position, const bool* first, const bool* last) {
    //     if (first == last) {
    //         return;
    //     }
    //     size_type n = 0;
    //     distance(first, last, n);
    //     if (capacity() - size() >= n) {
    //         copy_backward(position, end(), _M_finish + n);
    //         copy(first, last, position);
    //         _M_finish += n;
    //     } else {
    //         const size_type old_size = size();
    //         const size_type len = old_size + max(old_size, n);
    //         unsigned int* q = _M_bit_alloc(len);
    //         iterator i = copy(begin(), position, iterator(q, 0));
    //         i = copy(first, last, i);
    //         _M_finish = copy(position, end(), i);
    //         _M_deallocate();
    //         _M_end_of_storage = q + (len + WORD_BIT - 1) / WORD_BIT;
    //         _M_start = iterator(q, 0);
    //     }
    // }

    void _M_fill_insert(iterator position, size_type n, bool x) {
        if (n == 0) {
            return;
        }
        if (capacity() - size() >= n) {
            copy_backward(position, end(), _M_finish + difference_type(n));
            fill(position, position + n, x);
            _M_finish += difference_type(n);
        } else {
            const size_type old_size = size();
            const size_type len = old_size + max(old_size, n);
            unsigned int* q = _M_bit_alloc(len);
            iterator i = copy(begin(), position, iterator(q, 0));
            fill_n(i, n, x);
            _M_finish = copy(position, end(), i + n);
            _M_deallocate();
            _M_end_of_storage = q + (len + WORD_BIT - 1) / WORD_BIT;
            _M_start = iterator(q, 0);
        }
    }

    void insert(iterator position, size_type n, bool x) {
        _M_fill_insert(position, n, x);
    }

    void pop_back() {
        --_M_finish;
    }
    iterator erase(iterator position) {
        if (position + 1 != end()) {
            copy(position + 1, end(), position);
        }           
        --_M_finish;
        return position;
    }
    iterator erase(iterator first, iterator last) {
        _M_finish = copy(last, end(), first);
        return first;
    }
    void resize(size_type new_size, bool x = bool()) {
        if (new_size < size()) {
            erase(begin() + new_size, end());
        } else {
            insert(end(), new_size - size(), x);
        }
    }
    void flip() {
        for (unsigned int* p = _M_start._M_p; p != _M_end_of_storage; ++p) {
            *p = ~*p;
        }
    }
    void clear() {
        erase(begin(), end());
    }
};


SHADOW_STL_END_NAMESPACE

#endif // SHADOW_STL_INTERNAL_BVECTOR_H