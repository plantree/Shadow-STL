#ifndef SHADOW_STL_INTERNAL_VECTOR_H
#define SHADOW_STL_INTERNAL_VECTOR_H

#include "allocator/stl_alloc.h"
#include "include/type_traits.h"
#include "iterator/stl_iterator.h"
#include "stl_algobase.h"
#include "stl_construct.h"
#include "stl_iterator_base.h"
#include "stl_unitialized.h"
#include <cstddef>

SHADOW_STL_BEGIN_NAMESPACE

// The vector base class serves two purposes.  First, its constructor
// and destructor allocate (but don't initialize) storage.  This makes
// exception safety easier.  Second, the base class encapsulates all of
// the differences between SGI-style allocators and standard-conforming
// allocators.

// Base class for ordinary allocators.
template <typename T, typename Allocator, bool IsStatic>
class _Vector_alloc_base {
public:
  using allocator_type = typename _Alloc_traits<T, Allocator>::allocator_type;
  allocator_type get_allocator() const noexcept {
    return allocator_type(_M_data_allocator);
  }

  _Vector_alloc_base(const allocator_type &a) noexcept
      : _M_data_allocator(a), _M_start(nullptr), _M_finish(nullptr),
        _M_end_of_storage(nullptr) {}

protected:
  allocator_type _M_data_allocator;
  T *_M_start = nullptr;
  T *_M_finish = nullptr;
  T *_M_end_of_storage = nullptr;

  T *_M_allocate(size_t n) { return _M_data_allocator.allocate(n); }
  void _M_deallocate(T *p, size_t n) {
    if (p != nullptr) {
      _M_data_allocator.deallocate(p, n);
    }
  }
};

// Specialization for allocators that have the property that we don't
// actually have to store an allocator object.
template <typename T, typename Allocator>
class _Vector_alloc_base<T, Allocator, true> {
public:
  using allocator_type = typename _Alloc_traits<T, Allocator>::allocator_type;
  allocator_type get_allocator() const noexcept { return allocator_type(); }

  _Vector_alloc_base(const allocator_type &) noexcept
      : _M_start(nullptr), _M_finish(nullptr), _M_end_of_storage(nullptr) {}

protected:
  T *_M_start = nullptr;
  T *_M_finish = nullptr;
  T *_M_end_of_storage = nullptr;

  using _Alloc_traits_type = _Alloc_traits<T, Allocator>;
  T *_M_allocate(size_t n) { return _Alloc_traits_type::allocate(_M_start, n); }
  void _M_deallocate(T *p, size_t n) {
    _Alloc_traits_type::deallocate(_M_start, p, n);
  }
};

// template <typename T, typename Alloc>
// struct _Vector_base : public _Vector_alloc_base<T, Alloc, _Alloc_traits<T,
// Alloc>::_S_instanceless> {
//     using _Base = _Vector_alloc_base<T, Alloc, _Alloc_traits<T,
//     Alloc>::_S_instanceless>; using allocator_type = typename
//     _Base::allocator_type;

//     _Vector_base(const allocator_type& a) : _Base(a) {}
//     _Vector_base(size_t n, const allocator_type& a) : _Base(a) {
//         _M_start = _M_allocate(n);
//         _M_finish = _M_start;
//         _M_end_of_storage = _M_start + n;
//     }

//     ~_Vector_base() {
//         _M_deallocate(_M_start, _M_end_of_storage - _M_start);
//     }
// };

template <typename T, typename Alloc> class _Vector_base {
public:
  using allocator_type = Alloc;
  allocator_type get_allocator() const noexcept { return allocator_type(); }

  _Vector_base(const allocator_type &) noexcept
      : _M_start(nullptr), _M_finish(nullptr), _M_end_of_storage(nullptr) {}
  _Vector_base(size_t n, const allocator_type &) noexcept
      : _M_start(nullptr), _M_finish(nullptr), _M_end_of_storage(nullptr) {
    _M_start = _M_allocate(n);
    _M_finish = _M_start;
    _M_end_of_storage = _M_start + n;
  }

protected:
  T *_M_start = nullptr;
  T *_M_finish = nullptr;
  T *_M_end_of_storage = nullptr;

  using _M_data_allocator = simple_alloc<T, Alloc>;
  T *_M_allocate(size_t n) { return _M_data_allocator::allocate(n); }
  void _M_deallocate(T *p, size_t n) { _M_data_allocator::deallocate(p, n); }
};

template <typename T, typename Alloc = allocator<T>>
class vector : protected _Vector_base<T, Alloc> {
private:
  using _Base = _Vector_base<T, Alloc>;

public:
  using value_type = T;
  using pointer = T *;
  using const_pointer = const T *;
  using reference = value_type &;
  using const_reference = const value_type &;
  using size_type = size_t;
  using difference_type = ptrdiff_t;
  using iterator = value_type *;
  using const_iterator = const value_type *;

  using const_reverse_iterator = ::reverse_iterator<const_iterator>;
  using reverse_iterator = ::reverse_iterator<iterator>;

protected:
  using _Base::_M_allocate;
  using _Base::_M_deallocate;
  using _Base::_M_end_of_storage;
  using _Base::_M_finish;
  using _Base::_M_start;

  void _M_insert_aux(iterator position, const T &x);
  void _M_insert_aux(iterator position);

public:
  using allocator_type = Alloc;
  allocator_type get_allocator() const noexcept { return allocator_type(); }

  iterator begin() noexcept { return _M_start; }
  const_iterator begin() const noexcept { return _M_start; }
  iterator end() noexcept { return _M_finish; }
  const_iterator end() const noexcept { return _M_finish; }

  reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
  const_reverse_iterator rbegin() const noexcept {
    return const_reverse_iterator(end());
  }
  reverse_iterator rend() noexcept { return reverse_iterator(begin()); }
  const_reverse_iterator rend() const noexcept {
    return const_reverse_iterator(begin());
  }

  size_type size() const { return size_type(end() - begin()); }
  size_type max_size() const { return size_type(-1) / sizeof(T); }
  size_type capacity() const { return size_type(_M_end_of_storage - begin()); }
  bool empty() const noexcept { return begin() == end(); }

  reference operator[](size_type n) { return *(begin() + n); }
  const_reference operator[](size_type n) const { return *(begin() + n); }

  void _M_range_check(size_type n) const {
    if (n >= size()) {
      throw std::out_of_range("vector");
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

  explicit vector(const allocator_type &a = allocator_type()) noexcept
      : _Base(a) {}
  vector(size_type n, const T &value,
         const allocator_type &a = allocator_type())
      : _Base(n, a) {
    _M_finish = uninitialized_fill_n(_M_start, n, value);
  }

  explicit vector(size_type n) : _Base(n, allocator_type()) {
    _M_finish = uninitialized_fill_n(_M_start, n, T());
  }

  vector(const vector &x) : _Base(x.size(), x.get_allocator()) {
    _M_finish = uninitialized_copy(x.begin(), x.end(), _M_start);
  }

  // Check whether it's an integral type.  If so, it's not an iterator.
  template <typename InputIterator>
  vector(InputIterator first, InputIterator last,
         const allocator_type &a = allocator_type())
      : _Base(a) {
    using is_integral = typename _Is_integer<InputIterator>::_Integral;
    _M_initialize_aux(first, last, is_integral());
  }

  template <typename Integer>
  void _M_initialize_aux(Integer n, Integer value, _true_type) {
    _M_start = _M_allocate(static_cast<size_type>(n));
    _M_end_of_storage = _M_start + static_cast<size_type>(n);
    _M_finish = uninitialized_fill_n(_M_start, n, value);
  }
  template <typename InputIterator>
  void _M_initialize_aux(InputIterator first, InputIterator last, _false_type) {
    _M_range_initialize(first, last, iterator_category(first));
  }

  ~vector() { destroy(_M_start, _M_finish); }

  vector &operator=(const vector &x);

  void reserve(size_type n) {
    if (capacity() < n) {
      const size_type old_size = size();
      iterator tmp = _M_allocate_and_copy(n, _M_start, _M_finish);
      destroy(_M_start, _M_finish);
      _M_deallocate(_M_start, _M_end_of_storage - _M_start);
      _M_start = tmp;
      _M_finish = tmp + old_size;
      _M_end_of_storage = _M_start + n;
    }
  }

  // assign(), a generalized assignment member function.  Two
  // versions: one that takes a count, and one that takes a range.
  // The range version is a member template, so we dispatch on whether
  // or not the type is an integer.
  void assign(size_type n, const T &val) { _M_fill_assign(n, val); }
  void _M_fill_assign(size_type n, const T &val);

  template <typename InputIterator>
  void assign(InputIterator first, InputIterator last) {
    using is_integral = typename _Is_integer<InputIterator>::_Integral;
    _M_assign_dispatch(first, last, is_integral());
  }

  template <typename Integer>
  void _M_assign_dispatch(Integer n, Integer val, _true_type) {
    _M_fill_assign(static_cast<size_type>(n), static_cast<T>(val));
  }
  template <typename InputIterator>
  void _M_assign_dispatch(InputIterator first, InputIterator last,
                          _false_type) {
    _M_assign_aux(first, last, iterator_category(first));
  }

  template <typename InputIterator>
  void _M_assign_aux(InputIterator first, InputIterator last,
                     input_iterator_tag);
  template <typename ForwardIterator>
  void _M_assign_aux(ForwardIterator first, ForwardIterator last,
                     forward_iterator_tag);

  reference front() { return *begin(); }
  const_reference front() const { return *begin(); }
  reference back() { return *(end() - 1); }
  const_reference back() const { return *(end() - 1); }

  void push_back(const T &x) {
    if (_M_finish != _M_end_of_storage) {
      construct(_M_finish, x);
      ++_M_finish;
    } else {
      _M_insert_aux(end(), x);
    }
  }
  void push_back() {
    if (_M_finish != _M_end_of_storage) {
      construct(_M_finish);
      ++_M_finish;
    } else {
      _M_insert_aux(end());
    }
  }

  void swap(vector &x) noexcept {
    std::swap(_M_start, x._M_start);
    std::swap(_M_finish, x._M_finish);
    std::swap(_M_end_of_storage, x._M_end_of_storage);
  }

  // insert before `position`
  iterator insert(iterator position, const T &x) {
    size_type n = position - begin();
    if (_M_finish != _M_end_of_storage && position == end()) {
      construct(_M_finish, x);
      ++_M_finish;
    } else {
      _M_insert_aux(position, x);
    }
    return begin() + n;
  }
  iterator insert(iterator position) {
    size_type n = position - begin();
    if (_M_finish != _M_end_of_storage && position == end()) {
      construct(_M_finish);
      ++_M_finish;
    } else {
      _M_insert_aux(position);
    }
    return begin() + n;
  }

  // Check whether it's an integral type.  If so, it's not an iterator.
  template <typename InputIterator>
  void insert(iterator pos, InputIterator first, InputIterator last) {
    using is_integral = typename _Is_integer<InputIterator>::_Integral;
    _M_insert_dispatch(pos, first, last, is_integral());
  }

  void insert(iterator position, const_iterator first, const_iterator last);

  template <typename Integer>
  void _M_insert_dispatch(iterator pos, Integer n, Integer val, _true_type) {
    _M_fill_insert(pos, static_cast<size_type>(n), static_cast<T>(val));
  }
  template <typename InputIterator>
  void _M_insert_dispatch(iterator pos, InputIterator first, InputIterator last,
                          _false_type) {
    _M_range_insert(pos, first, last, iterator_category(first));
  }

  void insert(iterator pos, size_type n, const T &x) {
    _M_fill_insert(pos, n, x);
  }
  void _M_fill_insert(iterator pos, size_type n, const T &x);

  void pop_back() {
    --_M_finish;
    destroy(_M_finish);
  }

  iterator erase(iterator first, iterator last) {
    iterator i = copy(last, _M_finish, first);
    destroy(i, _M_finish);
    _M_finish = _M_finish - (last - first);
    return first;
  }
  iterator erase(iterator position) {
    if (position + 1 != end()) {
      copy(position + 1, _M_finish, position);
    }
    --_M_finish;
    destroy(_M_finish);
    return position;
  }

  void resize(size_type new_size, const T &x) {
    if (new_size < size()) {
      erase(begin() + new_size, end());
    } else {
      insert(end(), new_size - size(), x);
    }
  }
  void resize(size_type new_size) { resize(new_size, T()); }
  void clear() { erase(begin(), end()); }

protected:
  template <typename ForwardIterator>
  iterator _M_allocate_and_copy(size_type n, ForwardIterator first,
                                ForwardIterator last) {
    iterator result = _M_allocate(n);
    try {
      uninitialized_copy(first, last, result);
      return result;
    } catch (...) {
      _M_deallocate(result, n);
      throw;
    }
  }

  template <typename InputIterator>
  void _M_range_initialize(InputIterator first, InputIterator last,
                           input_iterator_tag) {
    for (; first != last; ++first) {
      push_back(*first);
    }
  }

  // This function is only called by the constructor.
  template <typename ForwardIterator>
  void _M_range_initialize(ForwardIterator first, ForwardIterator last,
                           forward_iterator_tag) {
    size_type n = distance(first, last);
    _M_start = _M_allocate(n);
    _M_end_of_storage = _M_start + n;
    _M_finish = uninitialized_copy(first, last, _M_start);
  }

  template <typename InputIterator>
  void _M_range_insert(iterator pos, InputIterator first, InputIterator last,
                       input_iterator_tag);
  template <typename ForwardIterator>
  void _M_range_insert(iterator pos, ForwardIterator first,
                       ForwardIterator last, forward_iterator_tag);
};

template <typename T, typename Alloc>
inline bool operator==(const vector<T, Alloc> &x, const vector<T, Alloc> &y) {
  return x.size() == y.size() && equal(x.begin(), x.end(), y.begin());
}

template <typename T, typename Alloc>
inline bool operator<(const vector<T, Alloc> &x, const vector<T, Alloc> &y) {
  return lexicographical_compare(x.begin(), x.end(), y.begin(), y.end());
}

template <typename T, typename Alloc>
inline void swap(vector<T, Alloc> &x, vector<T, Alloc> &y) noexcept {
  x.swap(y);
}

template <typename T, typename Alloc>
inline bool operator!=(const vector<T, Alloc> &x, const vector<T, Alloc> &y) {
  return !(x == y);
}

template <typename T, typename Alloc>
inline bool operator>(const vector<T, Alloc> &x, const vector<T, Alloc> &y) {
  return y < x;
}

template <typename T, typename Alloc>
inline bool operator<=(const vector<T, Alloc> &x, const vector<T, Alloc> &y) {
  return !(y < x);
}

template <typename T, typename Alloc>
inline bool operator>=(const vector<T, Alloc> &x, const vector<T, Alloc> &y) {
  return !(x < y);
}

template <typename T, typename Alloc>
vector<T, Alloc> &vector<T, Alloc>::operator=(const vector<T, Alloc> &x) {
  if (&x != this) {
    if (x.size() > capacity()) {
      iterator tmp = _M_allocate_and_copy(x.size(), x.begin(), x.end());
      destroy(_M_start, _M_finish);
      _M_deallocate(_M_start, _M_end_of_storage - _M_start);
      _M_start = tmp;
      _M_end_of_storage = _M_start + x.size();
    } else if (size() >= x.size()) {
      iterator i = copy(x.begin(), x.end(), begin());
      destroy(i, _M_finish);
    } else {
      copy(x.begin(), x.begin() + size(), _M_start);
      uninitialized_copy(x.begin() + size(), x.end(), _M_finish);
    }
    _M_finish = _M_start + x.size();
  }
  return *this;
}

template <typename T, typename Alloc>
void vector<T, Alloc>::_M_fill_assign(size_t n, const value_type &val) {
  if (n > capacity()) {
    vector<T, Alloc> tmp(n, val, get_allocator());
    tmp.swap(*this);
  } else if (n > size()) {
    fill(begin(), end(), val);
    _M_finish = uninitialized_fill_n(_M_finish, n - size(), val);
  } else {
    erase(fill_n(begin(), n, val), end());
  }
}

template <typename T, typename Alloc>
template <typename InputIterator>
void vector<T, Alloc>::_M_assign_aux(InputIterator first, InputIterator last,
                                     input_iterator_tag) {
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

template <typename T, typename Alloc>
template <typename ForwardIterator>
void vector<T, Alloc>::_M_assign_aux(ForwardIterator first,
                                     ForwardIterator last,
                                     forward_iterator_tag) {
  size_type len = distance(first, last);
  if (len > capacity()) {
    iterator tmp = _M_allocate_and_copy(len, first, last);
    destroy(_M_start, _M_finish);
    _M_deallocate(_M_start, _M_end_of_storage - _M_start);
    _M_start = tmp;
    _M_end_of_storage = _M_start + len;
  } else if (size() >= len) {
    iterator new_finish = copy(first, last, _M_start);
    destroy(new_finish, _M_finish);
    _M_finish = new_finish;
  } else {
    ForwardIterator mid = first;
    advance(mid, size());
    copy(first, mid, _M_start);
    _M_finish = uninitialized_copy(mid, last, _M_finish);
  }
}

template <typename T, typename Alloc>
void vector<T, Alloc>::_M_insert_aux(iterator position, const T &x) {
  if (_M_finish != _M_end_of_storage) {
    construct(_M_finish, *(_M_finish - 1));
    ++_M_finish;
    T x_copy = x;
    copy_backward(position, _M_finish - 2, _M_finish - 1);
    *position = x_copy;
  } else {
    const size_type old_size = size();
    const size_type len = old_size != 0 ? 2 * old_size : 1;
    iterator new_start = _M_allocate(len);
    iterator new_finish = new_start;
    try {
      new_finish = uninitialized_copy(_M_start, position, new_start);
      construct(new_finish, x);
      ++new_finish;
      new_finish = uninitialized_copy(position, _M_finish, new_finish);
    } catch (...) {
      destroy(new_start, new_finish);
      _M_deallocate(new_start, len);
    }
    destroy(begin(), end());
    _M_deallocate(_M_start, _M_end_of_storage - _M_start);
    _M_start = new_start;
    _M_finish = new_finish;
    _M_end_of_storage = new_start + len;
  }
}

template <typename T, typename Alloc>
void vector<T, Alloc>::_M_insert_aux(iterator position) {
  // delegate
  _M_insert_aux(position, T());
}

template <typename T, typename Alloc>
void vector<T, Alloc>::_M_fill_insert(iterator position, size_type n,
                                      const T &x) {
  if (n != 0) {
    if (size_type(_M_end_of_storage - _M_finish) >= n) {
      T x_copy = x;
      const size_type elems_after = _M_finish - position;
      iterator old_finish = _M_finish;
      if (elems_after > n) {
        uninitialized_copy(_M_finish - n, _M_finish, _M_finish);
        _M_finish += n;
        copy_backward(position, old_finish - n, old_finish);
        fill(position, position + n, x_copy);
      } else {
        uninitialized_fill_n(_M_finish, n - elems_after, x_copy);
        _M_finish += n - elems_after;
        uninitialized_copy(position, old_finish, _M_finish);
        _M_finish += elems_after;
        fill(position, old_finish, x_copy);
      }
    }
  } else {
    const size_type old_size = size();
    const size_type len = old_size + max(old_size, n);
    iterator new_start = _M_allocate(len);
    iterator new_finish = new_start;
    try {
      new_finish = uninitialized_copy(_M_start, position, new_start);
      new_finish = uninitialized_fill_n(new_finish, n, x);
      new_finish = uninitialized_copy(position, _M_finish, new_finish);
    } catch (...) {
      destroy(new_start, new_finish);
      _M_deallocate(new_start, len);
    }
    destroy(begin(), end());
    _M_deallocate(_M_start, _M_end_of_storage - _M_start);
    _M_start = new_start;
    _M_finish = new_finish;
    _M_end_of_storage = new_start + len;
  }
}

template <typename T, typename Alloc>
template <typename InputIterator>
void vector<T, Alloc>::_M_range_insert(iterator position, InputIterator first,
                                       InputIterator last, input_iterator_tag) {
  for (; first != last; ++first) {
    position = insert(position, *first);
    ++position;
  }
}

template <typename T, typename Alloc>
template <typename ForwardIterator>
void vector<T, Alloc>::_M_range_insert(iterator position, ForwardIterator first,
                                       ForwardIterator last,
                                       forward_iterator_tag) {
  if (first != last) {
    size_type n = 0;
    distance(first, last, n);
    if (size_type(_M_end_of_storage - _M_finish) >= n) {
      const size_type elems_after = _M_finish - position;
      iterator old_finish = _M_finish;
      if (elems_after > n) {
        uninitialized_copy(_M_finish - n, _M_finish, _M_finish);
        _M_finish += n;
        copy_backward(position, old_finish - n, old_finish);
        copy(first, last, position);
      } else {
        ForwardIterator mid = first;
        advance(mid, elems_after);
        uninitialized_copy(mid, last, _M_finish);
        _M_finish += n - elems_after;
        uninitialized_copy(position, old_finish, _M_finish);
        _M_finish += elems_after;
        copy(first, mid, position);
      }
    } else {
      const size_type old_size = size();
      const size_type len = old_size + max(old_size, n);
      iterator new_start = _M_allocate(len);
      iterator new_finish = new_start;
      try {
        new_finish = uninitialized_copy(_M_start, position, new_start);
        new_finish = uninitialized_copy(first, last, new_finish);
        new_finish = uninitialized_copy(position, _M_finish, new_finish);
      } catch (...) {
        destroy(new_start, new_finish);
        _M_deallocate(new_start, len);
      }
      destroy(begin(), end());
      _M_deallocate(_M_start, _M_end_of_storage - _M_start);
      _M_start = new_start;
      _M_finish = new_finish;
      _M_end_of_storage = new_start + len;
    }
  }
}

template <typename T, typename Alloc>
void vector<T, Alloc>::insert(iterator position, const_iterator first,
                              const_iterator last) {
  if (first != last) {
    size_type n = 0;
    distance(first, last, n);
    if (size_type(_M_end_of_storage - _M_finish) >= n) {
      const size_type elems_after = _M_finish - position;
      iterator old_finish = _M_finish;
      if (elems_after > n) {
        uninitialized_copy(_M_finish - n, _M_finish, _M_finish);
        _M_finish += n;
        copy_backward(position, old_finish - n, old_finish);
        copy(first, last, position);
      } else {
        uninitialized_copy(first + elems_after, last, _M_finish);
        _M_finish += n - elems_after;
        uninitialized_copy(position, old_finish, _M_finish);
        _M_finish += elems_after;
        copy(first, first + elems_after, position);
      }
    } else {
      const size_type old_size = size();
      const size_type len = old_size + max(old_size, n);
      iterator new_start = _M_allocate(len);
      iterator new_finish = new_start;
      try {
        new_finish = uninitialized_copy(_M_start, position, new_start);
        new_finish = uninitialized_copy(first, last, new_finish);
        new_finish = uninitialized_copy(position, _M_finish, new_finish);
      } catch (...) {
        destroy(new_start, new_finish);
        _M_deallocate(new_start, len);
      }
      destroy(begin(), end());
      _M_deallocate(_M_start, _M_end_of_storage - _M_start);
      _M_start = new_start;
      _M_finish = new_finish;
      _M_end_of_storage = new_start + len;
    }
  }
}

SHADOW_STL_END_NAMESPACE

#endif // SHADOW_STL_INTERNAL_VECTOR_H