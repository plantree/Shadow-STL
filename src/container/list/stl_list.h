#ifndef SHADOW_STL_INTERNAL_LIST_H
#define SHADOW_STL_INTERNAL_LIST_H

#include "allocator/stl_alloc.h"
#include "allocator/stl_construct.h"
#include "include/type_traits.h"
#include "iterator/stl_iterator.h"
#include "iterator/stl_iterator_base.h"
#include <cstddef>

SHADOW_STL_BEGIN_NAMESPACE

// bidirectional list node base
struct List_node_base {
  List_node_base *_M_next;
  List_node_base *_M_prev;
};

template <typename T> struct List_node : public List_node_base {
  T _M_data;
};

struct List_iterator_base {
  using size_type = size_t;
  using difference_type = ptrdiff_t;
  using iterator_category = bidirectional_iterator_tag;

  List_node_base *_M_node;

  List_iterator_base(List_node_base *x) : _M_node(x) {}
  List_iterator_base() {}

  void _M_incr() { _M_node = _M_node->_M_next; }
  void _M_decr() { _M_node = _M_node->_M_prev; }

  bool operator==(const List_iterator_base &__x) const {
    return _M_node == __x._M_node;
  }
  bool operator!=(const List_iterator_base &__x) const {
    return _M_node != __x._M_node;
  }
};

template <typename T, typename Ref, typename Ptr>
struct List_iterator : public List_iterator_base {
  using iterator = List_iterator<T, T &, T *>;
  using const_iterator = List_iterator<T, const T &, const T *>;
  using self = List_iterator<T, Ref, Ptr>;

  using value_type = T;
  using pointer = Ptr;
  using reference = Ref;

  using Node = List_node<T>;

  List_iterator() {}
  List_iterator(Node *x) : List_iterator_base(x) {}
  List_iterator(const iterator &x) : List_iterator_base(x._M_node) {}

  reference operator*() const { return static_cast<Node *>(_M_node)->_M_data; }
  pointer operator->() const { return &(operator*()); }

  self &operator++() {
    this->_M_incr();
    return *this;
  }
  self operator++(int) {
    self tmp = *this;
    this->_M_incr();
    return tmp;
  }
  self &operator--() {
    this->_M_decr();
    return *this;
  }
  self operator--(int) {
    self tmp = *this;
    this->_M_decr();
    return tmp;
  }
};

inline bidirectional_iterator_tag
iterator_category(const List_iterator_base &) {
  return bidirectional_iterator_tag();
}

template <typename T, typename Ref, typename Ptr>
inline T *value_type(const List_iterator<T, Ref, Ptr> &) {
  return nullptr;
}

inline ptrdiff_t *distance_type(const List_iterator_base &) { return nullptr; }

// Base class that encapsulates details of allocators.  Three cases:
// an ordinary standard-conforming allocator, a standard-conforming
// allocator with no non-static data, and an SGI-style allocator.
// This complexity is necessary only because we're worrying about backward
// compatibility and because we want to avoid wasting storage on an
// allocator instance if it isn't necessary.

// Base for general standard-conforming allocators.
template <typename T, typename Allocator, bool IsStatic> class List_alloc_base {
public:
  using allocator_type = typename _Alloc_traits<T, Allocator>::allocator_type;

  allocator_type get_allocator() const { return _Node_allocator; }

  List_alloc_base(const allocator_type &a) : _Node_allocator(a) {}

protected:
  List_node<T> *_M_get_node() { return _Node_allocator.allocate(1); }
  void _M_put_node(List_node<T> *p) { _Node_allocator.deallocate(p, 1); }

  allocator_type _Node_allocator;
  List_node<T> *_M_node;
};

// Specialization for instanceless allocators.
template <typename T, typename Allocator>
class List_alloc_base<T, Allocator, true> {
public:
  using allocator_type = typename _Alloc_traits<T, Allocator>::allocator_type;

  allocator_type get_allocator() const { return allocator_type(); }

  List_alloc_base(const allocator_type &) {}

protected:
  using _Node_Alloc_type =
      typename _Alloc_traits<List_node<T>, Allocator>::_Alloc_type;

  List_node<T> *_M_get_node() { return _Node_Alloc_type::allocate(1); }
  void _M_put_node(List_node<T> *p) { _Node_Alloc_type::deallocate(p, 1); }

  List_node<T> *_M_node;
};

template <typename T, typename Alloc>
class List_base
    : public List_alloc_base<T, Alloc,
                             _Alloc_traits<T, Alloc>::_S_instanceless> {
public:
  using Base =
      List_alloc_base<T, Alloc, _Alloc_traits<T, Alloc>::_S_instanceless>;
  using allocator_type = typename Base::allocator_type;
  List_base(const allocator_type &a) : Base(a) {
    _M_node = _M_get_node();
    _M_node->_M_next = _M_node;
    _M_node->_M_prev = _M_node;
  }
  ~List_base() {
    clear();
    _M_put_node(_M_node);
  }

  void clear();

protected:
  using Base::_M_get_node;
  using Base::_M_node;
  using Base::_M_put_node;
};

template <typename T, typename Alloc> void List_base<T, Alloc>::clear() {
  List_node<T> *cur = static_cast<List_node<T> *>(_M_node->_M_next);
  while (cur != _M_node) {
    List_node<T> *tmp = cur;
    cur = static_cast<List_node<T> *>(cur->_M_next);
    destroy(&tmp->_M_data);
    _M_put_node(tmp);
  }
  _M_node->_M_next = _M_node;
  _M_node->_M_prev = _M_node;
}

template <typename T, typename Alloc = allocator<T>>
class list : protected List_base<T, Alloc> {
  using Base = List_base<T, Alloc>;

protected:
  using Void_pointer = void *;

public:
  using value_type = T;
  using pointer = T *;
  using const_pointer = const T *;
  using reference = T &;
  using const_reference = const T &;
  using Node = List_node<T>;
  using size_type = size_t;
  using difference_type = ptrdiff_t;

  using allocator_type = typename Base::allocator_type;
  allocator_type get_allocator() const { return Base::get_allocator(); }

  using iterator = List_iterator<T, T &, T *>;
  using const_iterator = List_iterator<T, const T &, const T *>;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

protected:
  using Base::_M_get_node;
  using Base::_M_node;
  using Base::_M_put_node;

  Node *_M_create_node(const T &x) {
    Node *p = _M_get_node();
    try {
      construct(&p->_M_data, x);
    } catch (...) {
      _M_put_node(p);
      throw;
    }
    return p;
  }

  Node *_M_create_node() {
    Node *p = _M_get_node();
    try {
      construct(&p->_M_data);
    } catch (...) {
      _M_put_node(p);
      throw;
    }
    return p;
  }

public:
  explicit list(const allocator_type &a = allocator_type()) : Base(a) {}

  iterator begin() { return static_cast<Node *>(_M_node->_M_next); }
  const_iterator begin() const { return static_cast<Node *>(_M_node->_M_next); }

  iterator end() { return _M_node; }
  const_iterator end() const { return _M_node; }

  reverse_iterator rbegin() { return reverse_iterator(end()); }
  const_reverse_iterator rbegin() const {
    return const_reverse_iterator(end());
  }

  reverse_iterator rend() { return reverse_iterator(begin()); }
  const_reverse_iterator rend() const {
    return const_reverse_iterator(begin());
  }

  bool empty() const { return _M_node->_M_next == _M_node; }

  size_type size() const {
    size_type result = distance(begin(), end());
    return result;
  }

  size_type max_size() const { return size_type(-1); }

  reference front() { return *begin(); }
  const_reference front() const { return *begin(); }

  reference back() { return *(--end()); }
  const_reference back() const { return *(--end()); }

  void swap(list<T, Alloc> &x) { swap(_M_node, x._M_node); }

  iterator insert(iterator position, const T &x) {
    Node *tmp = _M_create_node(x);
    tmp->_M_next = position._M_node;
    tmp->_M_prev = position._M_node->_M_prev;
    position._M_node->_M_prev->_M_next = tmp;
    position._M_node->_M_prev = tmp;
    return tmp;
  }

  iterator insert(iterator position) { return insert(position, T()); }

  // Check whether it's an integral type.  If so, it's not an iterator.
  template <typename Integer>
  void _M_insert_dispatch(iterator pos, iterator n, Integer x, _true_type) {
    _M_fill_insert(pos, static_cast<size_type>(n), static_cast<T>(x));
  }

  template <typename InputIterator>
  void _M_insert_dispatch(iterator pos, InputIterator first, InputIterator last,
                          _false_type);

  template <typename InputIterator>
  void insert(iterator pos, InputIterator first, InputIterator last) {
    using is_integer = typename _Is_integer<InputIterator>::_Integral;
    _M_insert_dispatch(pos, first, last, is_integer());
  }

  void insert(iterator pos, size_type n, const T &x) {
    _M_fill_insert(pos, n, x);
  }

  void _M_fill_insert(iterator pos, size_type n, const T &x);

  void push_front(const T &x) { insert(begin(), x); }
  void push_front() { insert(begin()); }

  void push_back(const T &x) { insert(end(), x); }
  void push_back() { insert(end()); }

  iterator erase(iterator position) {
    Node *next_node = static_cast<Node *>(position._M_node->_M_next);
    Node *prev_node = static_cast<Node *>(position._M_node->_M_prev);
    prev_node->_M_next = next_node;
    next_node->_M_prev = prev_node;
    Node *n = static_cast<Node *>(position._M_node);
    destroy(&n->_M_data);
    _M_put_node(n);
    return iterator(next_node);
  }
  iterator erase(iterator first, iterator last);

  void clear() { Base::clear(); }

  void resize(size_type new_size, const T &x);
  void resize(size_type new_size) { this->resize(new_size, T()); }

  void pop_front() { erase(begin()); }
  void pop_back() {
    iterator tmp = end();
    erase(--tmp);
  }

  list(size_type n, const T &value, const allocator_type &a = allocator_type())
      : Base(a) {
    insert(begin(), n, value);
  }
  explicit list(size_type n) : Base(allocator_type()) {
    insert(begin(), n, T());
  }

  // We don't need any dispatching tricks here, because insert does all of
  // that anyway.
  template <typename InputIterator>
  list(InputIterator first, InputIterator last,
       const allocator_type &a = allocator_type())
      : Base(a) {
    insert(begin(), first, last);
  }
  list(const list<T, Alloc> &x) : Base(x.get_allocator()) {
    insert(begin(), x.begin(), x.end());
  }
  ~list() {}

  list<T, Alloc> &operator=(const list<T, Alloc> &x);

  // assign(), a generalized assignment member function.  Two
  // versions: one that takes a count, and one that takes a range.
  // The range version is a member template, so we dispatch on whether
  // or not the type is an integer.
  void assign(size_type n, const T &val) { _M_fill_assign(n, val); }

  void _M_fill_assign(size_type n, const T &val);

  template <typename InputIterator>
  void assign(InputIterator first, InputIterator last) {
    using is_integer = typename _Is_integer<InputIterator>::_Integral;
    _M_assign_dispatch(first, last, is_integer());
  }

  template <typename InputIterator>
  void _M_assign_dispatch(InputIterator first, InputIterator last, _false_type);

protected:
  void transfer(iterator position, iterator first, iterator last) {
    if (position != last) {
      last._M_node->_M_prev->_M_next = position._M_node;
      first._M_node->_M_prev->_M_next = last._M_node;
      position._M_node->_M_prev->_M_next = first._M_node;

      Node *tmp = position._M_node->_M_prev;
      position._M_node->_M_prev = last._M_node->_M_prev;
      last._M_node->_M_prev = first._M_node->_M_prev;
      first._M_node->_M_prev = tmp;
    }
  }

public:
  void splice(iterator position, list &x) {
    if (!x.empty()) {
      this->transfer(position, begin(), end());
    }
  }

  void splice(iterator position, list &, iterator i) {
    iterator j = i;
    ++j;
    if (position == i || position == j)
      return;
    this->transfer(position, i, j);
  }

  void splice(iterator position, list &, iterator first, iterator last) {
    if (first != last) {
      this->transfer(position, first, last);
    }
  }

  void remove(const T &value);
  void unique();
  void merge(list &x);
  void reverse();
  void sort();

  template <typename Predicate> void remove_if(Predicate);
  template <typename BinaryPredicate> void unique(BinaryPredicate);
  template <typename StrictWeakOrdering> void merge(list &, StrictWeakOrdering);
  template <typename StrictWeakOrdering> void sort(StrictWeakOrdering);
};

template <typename T, typename Alloc>
inline bool operator==(const list<T, Alloc> &x, const list<T, Alloc> &y) {
  using const_iterator = typename list<T, Alloc>::const_iterator;
  const_iterator end1 = x.end();
  const_iterator end2 = y.end();

  const_iterator i1 = x.begin();
  const_iterator i2 = y.begin();

  while (i1 != end1 && i2 != end2 && *i1 != *i2) {
    ++i1;
    ++i2;
  }
  return i1 == end1 && i2 == end2;
}

template <typename T, typename Alloc>
inline bool operator<(const list<T, Alloc> &x, const list<T, Alloc> &y) {
  return lexicographical_compare(x.begin(), x.end(), y.begin(), y.end());
}

template <typename T, typename Alloc>
inline bool operator!=(const list<T, Alloc> &x, const list<T, Alloc> &y) {
  return !(x == y);
}

template <typename T, typename Alloc>
inline bool operator>(const list<T, Alloc> &x, const list<T, Alloc> &y) {
  return y < x;
}

template <typename T, typename Alloc>
inline bool operator<=(const list<T, Alloc> &x, const list<T, Alloc> &y) {
  return !(y < x);
}

template <typename T, typename Alloc>
inline bool operator>=(const list<T, Alloc> &x, const list<T, Alloc> &y) {
  return !(x < y);
}

template <typename T, typename Alloc>
inline void swap(list<T, Alloc> &x, list<T, Alloc> &y) {
  x.swap(y);
}

template <typename T, typename Alloc>
template <typename InputIterator>
void list<T, Alloc>::_M_insert_dispatch(iterator pos, InputIterator first,
                                        InputIterator last, _false_type) {
  for (; first != last; ++first) {
    insert(pos, *first);
  }
}

template <typename T, typename Alloc>
void list<T, Alloc>::_M_fill_insert(iterator pos, size_type n, const T &x) {
  for (; n > 0; --n) {
    insert(pos, x);
  }
}

template <typename T, typename Alloc>
typename list<T, Alloc>::iterator list<T, Alloc>::erase(iterator first,
                                                        iterator last) {
  while (first != last) {
    erase(first++);
  }
  return last;
}

template <typename T, typename Alloc>
void list<T, Alloc>::resize(size_type new_size, const T &x) {
  iterator i = begin();
  size_type len = 0;
  for (; i != end() && len < new_size; ++i, ++len)
    ;
  if (len == new_size) {
    erase(i, end());
  } else {
    insert(end(), new_size - len, x);
  }
}

template <typename T, typename Alloc>
list<T, Alloc> &list<T, Alloc>::operator=(const list<T, Alloc> &x) {
  if (this != &x) {
    iterator first1 = begin();
    iterator last1 = end();
    const_iterator first2 = x.begin();
    const_iterator last2 = x.end();
    while (first1 != last1 && first2 != last2) {
      *first1++ = *first2++;
    }
    if (first2 == last2) {
      erase(first1, last1);
    } else {
      insert(last1, first2, last2);
    }
  }
  return *this;
}

template <typename T, typename Alloc>
void list<T, Alloc>::_M_fill_assign(size_type n, const T &val) {
  iterator i = begin();
  for (; i != end() && n > 0; ++i, --n) {
    *i = val;
  }
  if (n > 0) {
    insert(end(), n, val);
  } else {
    erase(i, end());
  }
}

template <typename T, typename Alloc>
template <typename InputIterator>
void list<T, Alloc>::_M_assign_dispatch(InputIterator first, InputIterator last,
                                        _false_type) {
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
void list<T, Alloc>::remove(const T &val) {
  iterator first = begin();
  iterator last = end();
  while (first != last) {
    iterator next = first;
    ++next;
    if (*first == val) {
      erase(first);
    }
    first = next;
  }
}

template <typename T, typename Alloc> void list<T, Alloc>::unique() {
  iterator first = begin();
  iterator last = end();
  if (first == last)
    return;
  iterator next = first;
  while (++next != last) {
    if (*first == *next) {
      erase(next);
    } else {
      first = next;
    }
    next = first;
  }
}

template <typename T, typename Alloc>
void list<T, Alloc>::merge(list<T, Alloc> &x) {
  iterator first1 = begin();
  iterator last1 = end();
  iterator first2 = x.begin();
  iterator last2 = x.end();
  while (first1 != last1 && first2 != last2) {
    if (*first1 > *first2) {
      iterator next = first2;
      transfer(first1, first2, ++next);
      first2 = next;
    } else {
      ++first1;
    }
  }
  if (first2 != last2) {
    transfer(last1, first2, last2);
  }
}

inline void _List_base_reverse(List_node_base *p) {
  List_node_base *tmp = p;
  do {
    std::swap(tmp->_M_next, tmp->_M_prev);
    tmp = tmp->_M_prev;
  } while (tmp != p);
}

template <typename T, typename Alloc> void list<T, Alloc>::reverse() {
  _List_base_reverse(_M_node);
}

// merge sort
template <typename T, typename Alloc> void list<T, Alloc>::sort() {
  // Do nothing if the list has length 0 or 1.
  if (_M_node->_M_next != _M_node && _M_node->_M_next->_M_next != _M_node) {
    list<T, Alloc> carry;
    list<T, Alloc> counter[64];
    int fill = 0;
    while (!empty()) {
      carry.splice(carry.begin(), *this, begin());
      int i = 0;
      while (i < fill && !counter[i].empty()) {
        counter[i].merge(carry);
        carry.swap(counter[i++]);
      }
      carry.swap(counter[i]);
      if (i == fill)
        ++fill;
    }

    for (int i = 1; i < fill; ++i) {
      counter[i].merge(counter[i - 1]);
    }
    swap(counter[fill - 1]);
  }
}

template <typename T, typename Alloc>
template <typename Predicate>
void list<T, Alloc>::remove_if(Predicate pred) {
  iterator first = begin();
  iterator last = end();
  while (first != last) {
    iterator next = first;
    ++next;
    if (pred(*first)) {
      erase(first);
    }
    first = next;
  }
}

template <typename T, typename Alloc>
template <typename BinaryPredicate>
void list<T, Alloc>::unique(BinaryPredicate binary_pred) {
  iterator first = begin();
  iterator last = end();
  if (first == last)
    return;
  iterator next = first;
  while (++next != last) {
    if (binary_pred(*first, next)) {
      erase(next);
    } else {
      first = next;
    }
    next = first;
  }
}

template <typename T, typename Alloc>
template <typename StrictWeakOrdering>
void list<T, Alloc>::merge(list<T, Alloc> &x, StrictWeakOrdering comp) {
  iterator first1 = begin();
  iterator last1 = end();
  iterator first2 = x.begin();
  iterator last2 = x.end();
  while (first1 != last1 && first2 != last2) {
    if (comp(*first2, *first1)) {
      iterator next = first2;
      transfer(first1, first2, ++next);
      first2 = next;
    } else {
      ++first1;
    }
  }
  if (first2 != last2) {
    transfer(last1, first2, last2);
  }
}

// merge sort
template <typename T, typename Alloc>
template <typename StrictWeakOrdering>
void list<T, Alloc>::sort(StrictWeakOrdering comp) {
  // Do nothing if the list has length 0 or 1.
  if (_M_node->_M_next != _M_node && _M_node->_M_next->_M_next != _M_node) {
    list<T, Alloc> carry;
    list<T, Alloc> counter[64];
    int fill = 0;
    while (!empty()) {
      carry.splice(carry.begin(), *this, begin());
      int i = 0;
      while (i < fill && !counter[i].empty()) {
        counter[i].merge(carry, comp);
        carry.swap(counter[i++]);
      }
      carry.swap(counter[i]);
      if (i == fill)
        ++fill;
    }

    for (int i = 1; i < fill; ++i) {
      counter[i].merge(counter[i - 1].comp);
    }
    swap(counter[fill - 1]);
  }
}

SHADOW_STL_END_NAMESPACE

#endif // SHADOW_STL_INTERNAL_LIST_H