#ifndef SHADOW_STL_INTERNAL_SLIST_H
#define SHADOW_STL_INTERNAL_SLIST_H

#include "allocator/stl_alloc.h"
#include "allocator/stl_construct.h"
#include "include/type_traits.h"
#include "iterator/stl_iterator_base.h"
#include <cstddef>

SHADOW_STL_BEGIN_NAMESPACE

struct Slist_node_base {
  Slist_node_base *_next;
};

inline Slist_node_base *_slist_make_link(Slist_node_base *prev_node,
                                         Slist_node_base *new_node) {
  new_node->_next = prev_node->_next;
  prev_node->_next = new_node;
  return new_node;
}

// find previous node
inline Slist_node_base *_slist_previous(Slist_node_base *head,
                                        const Slist_node_base *node) {
  while (head && head->_next != node) {
    head = head->_next;
  }
  return head;
}

inline const Slist_node_base *_slist_previous(const Slist_node_base *head,
                                              const Slist_node_base *node) {
  while (head && head->_next != node) {
    head = head->_next;
  }
  return head;
}

inline void _slist_splice_after(Slist_node_base *pos,
                                Slist_node_base *before_first,
                                Slist_node_base *before_last) {
  if (pos != before_first && pos != before_last) {
    Slist_node_base *first = before_first->_next;
    Slist_node_base *after = pos->_next;
    before_first->_next = before_last->_next;
    pos->_next = first;
    before_last->_next = after;
  }
}

inline void _slist_splice_after(Slist_node_base *pos, Slist_node_base *head) {
  Slist_node_base *before_last = _slist_previous(head, nullptr);
  if (before_last != head) {
    Slist_node_base *after = pos->_next;
    pos->_next = head->_next;
    head->_next = nullptr;
    before_last->_next = after;
  }
}

inline Slist_node_base *_slist_reverse(Slist_node_base *node) {
  Slist_node_base *result = node;
  node = node->_next;
  result->_next = nullptr;
  while (node) {
    Slist_node_base *_next = node->_next;
    node->_next = result;
    result = node;
    node = _next;
  }
  return result;
}

inline size_t _slist_size(Slist_node_base *node) {
  size_t result = 0;
  while (node) {
    ++result;
    node = node->_next;
  }
  return result;
}

template <typename T> struct Slist_node : public Slist_node_base {
  T _data;
};

struct Slist_iterator_base {
  using size_type = size_t;
  using difference_type = ptrdiff_t;
  using iterator_category = forward_iterator_tag;

  Slist_node_base *_node;

  Slist_iterator_base(Slist_node_base *x) : _node(x) {}
  void _M_incr() { _node = _node->_next; }

  bool operator==(const Slist_iterator_base &x) const {
    return _node == x._node;
  }
  bool operator!=(const Slist_iterator_base &x) const {
    return _node != x._node;
  }
};

template <typename T, typename Ref, typename Ptr>
struct Slist_iterator : public Slist_iterator_base {
  using iterator = Slist_iterator<T, T &, T *>;
  using const_iterator = Slist_iterator<T, const T &, const T *>;
  using self = Slist_iterator<T, Ref, Ptr>;

  using value_type = T;
  using pointer = Ptr;
  using reference = Ref;
  using Node = Slist_node<T>;

  Slist_iterator(Node *x) : Slist_iterator_base(x) {}
  Slist_iterator() : Slist_iterator_base(nullptr) {}
  Slist_iterator(const iterator &x) : Slist_iterator_base(x._node) {}

  reference operator*() const { return static_cast<Node *>(_node)->_data; }
  pointer operator->() const { return &(operator*()); }

  self &operator++() {
    _M_incr();
    return *this;
  }
  self operator++(int) {
    self tmp = *this;
    _M_incr();
    return tmp;
  }
};

inline ptrdiff_t *distance_type(const Slist_node_base &) {
  return static_cast<ptrdiff_t *>(0);
}

inline forward_iterator_tag iterator_category(const Slist_node_base &) {
  return forward_iterator_tag();
}

template <typename T, typename Ref, typename Ptr>
inline T *value_type(const Slist_iterator<T, Ref, Ptr> &) {
  return static_cast<T *>(0);
}

// Base class that encapsulates details of allocators.  Three cases:
// an ordinary standard-conforming allocator, a standard-conforming
// allocator with no non-static data, and an SGI-style allocator.
// This complexity is necessary only because we're worrying about backward
// compatibility and because we want to avoid wasting storage on an
// allocator instance if it isn't necessary.

// Base for general standard-conforming allocators.
template <typename T, typename Allocator, bool IsStatic>
class Slist_alloc_base {
public:
  using allocator_type = typename _Alloc_traits<T, Allocator>::allocator_type;
  allocator_type get_allocator() const { return allocator_type(); }

  Slist_alloc_base(const allocator_type &a) : _node_allocator(a) {}

protected:
  Slist_node<T> *_M_get_node() { return _node_allocator.allocate(1); }
  void _M_put_node(Slist_node<T> *p) { _node_allocator.deallocate(p, 1); }

  allocator_type _node_allocator;
  Slist_node_base _head;
};

// Specialization for instanceless allocators.
template <typename T, typename Allocator>
class Slist_alloc_base<T, Allocator, true> {
public:
  using allocator_type = typename _Alloc_traits<T, Allocator>::allocator_type;
  allocator_type get_allocator() const { return allocator_type(); }

  Slist_alloc_base(const allocator_type &) {}

protected:
  using Alloc_type =
      typename _Alloc_traits<Slist_node<T>, Allocator>::_Alloc_type;
  Slist_node<T> *_M_get_node() { return Alloc_type::allocate(1); }
  void _M_put_node(Slist_node<T> *p) { Alloc_type::deallocate(p, 1); }

  Slist_node_base _head;
};

// Base class that encapsulates details of iterator implementation.
template <typename T, typename Alloc>
struct Slist_base
    : public Slist_alloc_base<T, Alloc,
                              _Alloc_traits<T, Alloc>::_S_instanceless> {
  using Base =
      Slist_alloc_base<T, Alloc, _Alloc_traits<T, Alloc>::_S_instanceless>;
  using allocator_type = typename Base::allocator_type;
  using Base::_head;

  Slist_base(const allocator_type &a) : Base(a) { _head._next = nullptr; }
  ~Slist_base() { _M_erase_after(&_head, nullptr); }

protected:
  Slist_node_base *_M_erase_after(Slist_node_base *pos) {
    Slist_node<T> *next = static_cast<Slist_node<T> *>(pos->_next);
    Slist_node_base *next_next = next->_next;
    pos->_next = next_next;
    destroy(&next->_data);
    _M_put_node(next);
    return next_next;
  }
  Slist_node_base *_M_erase_after(Slist_node_base *, Slist_node_base *);
};

template <typename T, typename Alloc>
Slist_node_base *
Slist_base<T, Alloc>::_M_erase_after(Slist_node_base *before_first,
                                     Slist_node_base *last) {
  Slist_node<T> *cur = static_cast<Slist_node<T> *>(before_first->_next);
  while (cur != last) {
    Slist_node<T> *tmp = cur;
    cur = static_cast<Slist_node<T> *>(cur->_next);
    destroy(&tmp->_data);
    this->_M_put_node(tmp);
  }
  before_first->_next = last;
  return last;
}

template <typename T, typename Alloc = allocator<T>>
class slist : private Slist_base<T, Alloc> {
private:
  using Base = Slist_base<T, Alloc>;

public:
  using value_type = T;
  using pointer = T *;
  using const_pointer = const T *;
  using reference = T &;
  using const_reference = const T &;
  using size_type = size_t;
  using difference_type = ptrdiff_t;

  using iterator = Slist_iterator<T, T &, T *>;
  using const_iterator = Slist_iterator<T, const T &, const T *>;

  using allocator_type = typename Base::allocator_type;
  allocator_type get_allocator() const { return Base::get_allocator(); }

private:
  using Base::_head;
  using Base::_M_erase_after;
  using Base::_M_get_node;
  using Base::_M_put_node;

  using Node = Slist_node<T>;
  using Node_base = Slist_node_base;
  using Iterator_base = Slist_iterator_base;

  Node *_M_create_node(const value_type &x) {
    Node *node = _M_get_node();
    try {
      construct(&node->_data, x);
      node->_next = nullptr;
    } catch (...) {
      _M_put_node(node);
      throw;
    }
    return node;
  }

  Node *_M_create_node() {
    Node *node = _M_get_node();
    try {
      construct(&node->_data);
      node->_next = nullptr;
    } catch (...) {
      _M_put_node(node);
      throw;
    }
    return node;
  }

public:
  explicit slist(const allocator_type &a = allocator_type()) : Base(a) {}
  slist(size_type n, const value_type &x,
        const allocator_type &a = allocator_type())
      : Base(a) {
    _M_insert_after_fill(&_head, n, x);
  }

  // We don't need any dispatching tricks here, because _M_insert_after_range
  // already does them.
  template <typename InpuIterator>
  slist(InpuIterator first, InpuIterator last,
        const allocator_type &a = allocator_type())
      : Base(a) {
    _M_insert_after_range(&_head, first, last);
  }
  slist(const value_type *first, const value_type *last,
        const allocator_type &a = allocator_type())
      : Base(a) {
    _M_insert_after_range(&_head, first, last);
  }
  slist(const slist &x) : Base(x.get_allocator()) {
    _M_insert_after_range(&_head, x.begin(), x.end());
  }
  slist &operator=(const slist &x);
  ~slist() {}

  // assign(), a generalized assignment member function.  Two
  // versions: one that takes a count, and one that takes a range.
  // The range version is a member template, so we dispatch on whether
  // or not the type is an integer.
  void assign(size_type n, const T &val) { _M_fill_assign(n, val); }
  void _M_fill_assign(size_type n, const T &val);

  template <typename InpuIterator>
  void assign(InpuIterator first, InpuIterator last) {
    using Integral = typename _Is_integer<InpuIterator>::_Integral;
    _M_assign_dispatch(first, last, Integral());
  }

  template <typename Integer>
  void _M_assign_dispatch(Integer n, Integer val, _true_type) {
    _M_fill_assign((size_type)n, (T)val);
  }
  template <typename InputIterator>
  void _M_assign_dispatch(InputIterator first, InputIterator last, _false_type);

  iterator begin() { return iterator(static_cast<Node *>(_head._next)); }
  const_iterator begin() const {
    return const_iterator(static_cast<Node *>(_head._next));
  }

  // Experimental new feature: before_begin() returns a
  // non-dereferenceable iterator that, when incremented, yields
  // begin().  This iterator may be used as the argument to
  // insert_after, erase_after, etc.  Note that even for an empty
  // slist, before_begin() is not the same iterator as end().  It
  // is always necessary to increment before_begin() at least once to
  // obtain end().
  iterator before_begin() { return iterator(static_cast<Node *>(_head)); }
  const_iterator before_begin() const {
    return const_iterator(static_cast<Node *>(_head));
  }

  size_type size() const {
    return _slist_size(static_cast<Node_base *>(_head._next));
  }
  size_type max_size() const { return size_type(-1); }
  bool empty() const { return _head._next == nullptr; }
  void swap(slist &x) { std::swap(_head._next, x._head._next); }

  reference front() { return static_cast<Node *>(_head._next)->_data; }
  const_reference front() const {
    return static_cast<Node *>(_head._next)->_data;
  }
  void push_front(const value_type &x) {
    _slist_make_link(&_head, _M_create_node(x));
  }
  void push_front() { _slist_make_link(_head, _M_create_node()); }
  void pop_front() {
    Node *node = static_cast<Node *>(_head._next);
    _head._next = node->_next;
    destroy(&node->_data);
    _M_put_node(node);
  }

  iterator previous(const_iterator pos) {
    return iterator(static_cast<Node *>(
        _slist_previous(static_cast<Node *>(_head._next), pos._node)));
  }
  const_iterator previous(const_iterator pos) const {
    return const_iterator(static_cast<Node *>(
        _slist_previous(static_cast<Node *>(_head._next), pos._node)));
  }

private:
  Node *_M_insert_after(Node_base *pos, const value_type &x) {
    return static_cast<Node *>(_slist_make_link(pos, _M_create_node(x)));
  }
  Node *_M_insert_after(Node_base *pos) {
    return static_cast<Node *>(_slist_make_link(pos, _M_create_node()));
  }

  void _M_insert_after_fill(Node_base *pos, size_type n, const value_type &x) {
    for (size_type i = 0; i < n; ++i) {
      pos = _slist_make_link(pos, _M_create_node(x));
    }
  }

  // Check whether it's an integral type.  If so, it's not an iterator.
  template <typename InputIter>
  void _M_insert_after_range(Node_base *pos, InputIter first, InputIter last) {
    using Integral = typename _Is_integer<InputIter>::_Integral;
    _M_insert_after_range(pos, first, last, Integral());
  }
  template <typename Integer>
  void _M_insert_after_range(Node_base *pos, Integer n, Integer x, _true_type) {
    _M_insert_after_fill(pos, static_cast<size_type>(n), static_cast<T>(x));
  }
  template <typename InputIter>
  void _M_insert_after_range(Node_base *pos, InputIter first, InputIter last,
                             _false_type) {
    for (; first != last; ++first) {
      pos = _slist_make_link(pos, _M_create_node(*first));
    }
  }

public:
  iterator insert_after(iterator pos, const value_type &x) {
    return iterator(_M_insert_after(pos._node, x));
  }
  iterator insert_after(iterator pos) {
    return iterator(_M_insert_after(pos._node));
  }
  void insert_after(iterator pos, size_type n, const value_type &x) {
    _M_insert_after_fill(pos._node, n, x);
  }

  iterator insert(iterator pos) {
    return iterator(
        _M_insert_after(_slist_previous(&_head, pos._node), value_type()));
  }
  void insert(iterator pos, size_type n, const value_type &x) {
    _M_insert_after_fill(_slist_previous(_head, pos._node), n, x);
  }
  // We don't need any dispatching tricks here, because _M_insert_after_range
  // already does them.
  template <typename InpuIterator>
  void insert(iterator pos, InpuIterator first, InpuIterator last) {
    _M_insert_after_range(_slist_previous(&_head, pos._node), first, last);
  }

  iterator erase_after(iterator pos) {
    return iterator(static_cast<Node *>(_M_erase_after(pos._node)));
  }
  iterator erase_after(iterator before_first, iterator last) {
    return iterator(
        static_cast<Node *>(_M_erase_after(before_first._node, last._node)));
  }
  iterator erase(iterator pos) {
    return static_cast<Node *>(
        _M_erase_after(_slist_previous(&_head, pos._node)));
  }
  iterator erase(iterator first, iterator last) {
    return static_cast<Node *>(
        _M_erase_after(_slist_previous(&_head, first._node), last._node));
  }
  void resize(size_type new_size, const T &x);
  void resize(size_type new_size) { resize(new_size, T()); }
  void clear() { _M_erase_after(&_head, nullptr); }

  // Moves the range [__before_first + 1, __before_last + 1) to *this,
  //  inserting it immediately after __pos.  This is constant time.
  void splice_after(iterator pos, iterator before_first, iterator before_last) {
    if (before_first != before_last) {
      _slist_splice_after(pos._node, before_first._node, before_last._node);
    }
  }
  // Moves the element that follows __prev to *this, inserting it immediately
  //  after __pos.  This is constant time.
  void splice_after(iterator pos, iterator prev) {
    _slist_splice_after(pos._node, prev._node, prev._node->_next);
  }
  // Removes all of the elements from the list __x to *this, inserting
  // them immediately after __pos.  __x must not be *this.  Complexity:
  // linear in __x.size().
  void splice_after(iterator pos, slist &x) {
    _slist_splice_after(pos._node, &x._head);
  }
  // Linear in distance(begin(), __pos), and linear in __x.size().
  void splice(iterator pos, slist &x) {
    if (x._head._next) {
      _slist_splice_after(_slist_previous(&_head, pos._node), &x._head);
    }
  }
  // Linear in distance(begin(), __pos), and in distance(__x.begin(), __i).
  void splice(iterator pos, slist &x, iterator i) {
    _slist_splice_after(_slist_previous(&_head, pos._node),
                        _slist_previous(&x._head, i._node), i._node);
  }
  // Linear in distance(begin(), __pos), in distance(__x.begin(), __first),
  // and in distance(__first, __last).
  void splice(iterator pos, slist &x, iterator first, iterator last) {
    if (first != last) {
      _slist_splice_after(_slist_previous(&_head, pos._node),
                          _slist_previous(&x._head, first._node),
                          _slist_previous(first._node, last._node));
    }
  }

  void reverse() {
    if (_head._next) {
      _head._next = _slist_reverse(static_cast<Node *>(_head._next));
    }
  }

  void remove(const T &val);
  void unique();
  void merge(slist &x);
  void sort();

  template <typename Predicate> void remove_if(Predicate pred);

  template <typename BinaryPredicate> void unique(BinaryPredicate binary_pred);

  template <typename StrictWeakOrdering>
  void merge(slist &x, StrictWeakOrdering comp);

  template <typename StrictWeakOrdering> void sort(StrictWeakOrdering comp);
};

template <typename T, typename Alloc>
slist<T, Alloc> &slist<T, Alloc>::operator=(const slist<T, Alloc> &x) {
  if (&x != this) {
    Node_base *p1 = &_head;
    Node *n1 = static_cast<Node *>(_head._next);
    const Node *n2 = static_cast<const Node *>(x._head._next);
    while (n1 && n2) {
      n1->_data = n2->_data;
      p1 = n1;
      n1 = static_cast<Node *>(n1->_next);
      n2 = static_cast<const Node *>(n2->_next);
    }
    if (n2 == nullptr) {
      _M_erase_after(p1, nullptr);
    } else {
      _M_insert_after_range(p1, const_iterator(static_cast<Node *>(n2)),
                            const_iterator(nullptr));
    }
  }
  return *this;
}

template <typename T, typename Alloc>
void slist<T, Alloc>::_M_fill_assign(size_type n, const T &val) {
  Node_base *prev = &_head;
  Node *cur = static_cast<Node *>(_head._next);
  for (; cur && n > 0; --n) {
    cur->_data = val;
    prev = cur;
    cur = static_cast<Node *>(cur->_next);
  }
  if (n > 0) {
    _M_insert_after_fill(prev, n, val);
  } else {
    _M_erase_after(prev, nullptr);
  }
}

template <typename T, typename Alloc>
template <typename InputIterator>
void slist<T, Alloc>::_M_assign_dispatch(InputIterator first,
                                         InputIterator last, _false_type) {
  Node_base *prev = &_head;
  Node *cur = static_cast<Node *>(_head._next);
  while (cur && first != last) {
    cur->_data = *first;
    prev = cur;
    cur = static_cast<Node *>(cur->_next);
    ++first;
  }
  if (first != last) {
    _M_insert_after_range(prev, first, last);
  } else {
    _M_erase_after(prev, nullptr);
  }
}

template <typename T, typename Alloc>
inline bool operator==(const slist<T, Alloc> &sl1, const slist<T, Alloc> &sl2) {
  using const_iterator = typename slist<T, Alloc>::const_iterator;
  const_iterator end1 = sl1.end();
  const_iterator end2 = sl2.end();
  const_iterator i1 = sl1.begin();
  const_iterator i2 = sl2.begin();
  while (i1 != end1 && i2 != end2 && *i1 == *i2) {
    ++i1;
    ++i2;
  }
  return i1 == end1 && i2 == end2;
}

template <typename T, typename Alloc>
inline bool operator<(const slist<T, Alloc> &sl1, const slist<T, Alloc> &sl2) {
  return lexicographical_compare(sl1.begin(), sl1.end(), sl2.begin(),
                                 sl2.end());
}

template <typename T, typename Alloc>
inline bool operator!=(const slist<T, Alloc> &sl1, const slist<T, Alloc> &sl2) {
  return !(sl1 == sl2);
}

template <typename T, typename Alloc>
inline bool operator>(const slist<T, Alloc> &sl1, const slist<T, Alloc> &sl2) {
  return sl2 < sl1;
}

template <typename T, typename Alloc>
inline bool operator<=(const slist<T, Alloc> &sl1, const slist<T, Alloc> &sl2) {
  return !(sl2 < sl1);
}

template <typename T, typename Alloc>
inline bool operator>=(const slist<T, Alloc> &sl1, const slist<T, Alloc> &sl2) {
  return !(sl1 < sl2);
}

template <typename T, typename Alloc>
inline void swap(slist<T, Alloc> &x, slist<T, Alloc> &y) {
  x.swap(y);
}

template <typename T, typename Alloc>
void slist<T, Alloc>::resize(size_type new_size, const T &x) {
  Node_base *cur = &_head;
  while (cur->_next && new_size > 0) {
    cur = cur->_next;
    --new_size;
  }
  if (cur->_next) {
    _M_erase_after(cur, nullptr);
  } else {
    _M_insert_after_fill(cur, new_size, x);
  }
}

template <typename T, typename Alloc>
void slist<T, Alloc>::remove(const T &val) {
  Node_base *cur = &_head;
  while (cur && cur->_next) {
    if (static_cast<Node *>(cur->_next)->_data == val) {
      _M_erase_after(cur);
    } else {
      cur = cur->_next;
    }
  }
}

template <typename T, typename Alloc> void slist<T, Alloc>::unique() {
  Node_base *cur = _head._next;
  if (cur) {
    while (cur->_next) {
      if (static_cast<Node *>(cur)->_data ==
          static_cast<Node *>(cur->_next)->_data) {
        _M_erase_after(cur);
      } else {
        cur = cur->_next;
      }
    }
  }
}

template <typename T, typename Alloc>
void slist<T, Alloc>::merge(slist<T, Alloc> &x) {
  Node_base *n1 = &_head;
  while (n1->_next && x._head._next) {
    if (static_cast<Node *>(x._head._next)->_data <
        static_cast<Node *>(n1->_next)->_data) {
      _slist_splice_after(n1, &x._head, x._head._next);
    }
    n1 = n1->_next;
  }
  if (x._head._next) {
    n1->_next = x._head._next;
    x._head._next = nullptr;
  }
}

template <typename T, typename Alloc> void slist<T, Alloc>::sort() {
  if (_head._next && _head._next->_next) {
    slist<T, Alloc> carry;
    slist<T, Alloc> counter[64];
    int fill = 0;
    while (!empty()) {
      _slist_splice_after(&carry._head, &_head, _head._next);
      int i = 0;
      while (i < fill && !counter[i].empty()) {
        counter[i].merge(carry);
        carry.swap(counter[i++]);
      }
      carry.swap(counter[i]);
      if (i == fill) {
        ++fill;
      }
    }

    for (int i = 1; i < fill; ++i) {
      counter[i].merge(counter[i - 1]);
    }
    swap(counter[fill - 1]);
  }
}

template <typename T, typename Alloc>
template <typename Predicate>
void slist<T, Alloc>::remove_if(Predicate pred) {
  Node_base *cur = &_head;
  while (cur && cur->_next) {
    if (pred(static_cast<Node *>(cur->_next)->_data)) {
      _M_erase_after(cur);
    } else {
      cur = cur->_next;
    }
  }
}

template <typename T, typename Alloc>
template <typename BinaryPredicate>
void slist<T, Alloc>::unique(BinaryPredicate binary_pred) {
  Node_base *cur = &_head;
  while (cur && cur->_next) {
    if (binary_pred(static_cast<Node *>(cur->_next)->_data,
                    static_cast<Node *>(cur->_next->_next)->_data)) {
      _M_erase_after(cur);
    } else {
      cur = cur->_next;
    }
  }
}

template <typename T, typename Alloc>
template <typename StrictWeakOrdering>
void slist<T, Alloc>::merge(slist<T, Alloc> &x, StrictWeakOrdering comp) {
  Node_base *n1 = &_head;
  while (n1->_next && x._head._next) {
    if (comp(static_cast<Node *>(x._head._next)->_data,
             static_cast<Node *>(n1->_next)->_data)) {
      _slist_splice_after(n1, &x._head, x._head._next);
    }
    n1 = n1->_next;
  }
  if (x._head._next) {
    n1->_next = x._head._next;
    x._head._next = nullptr;
  }
}

template <typename T, typename Alloc>
template <typename StrictWeakOrdering>
void slist<T, Alloc>::sort(StrictWeakOrdering comp) {
  if (_head._next && _head._next->_next) {
    slist<T, Alloc> carry;
    slist<T, Alloc> counter[64];
    int fill = 0;
    while (!empty()) {
      _slist_splice_after(&carry._head, &_head, _head._next);
      int i = 0;
      while (i < fill && !counter[i].empty()) {
        counter[i].merge(carry, comp);
        carry.swap(counter[i++]);
      }
      carry.swap(counter[i]);
      if (i == fill) {
        ++fill;
      }
    }

    for (int i = 1; i < fill; ++i) {
      counter[i].merge(counter[i - 1]);
    }
    swap(counter[fill - 1]);
  }
}

SHADOW_STL_END_NAMESPACE

#endif // SHADOW_STL_INTERNAL_SLIST_H