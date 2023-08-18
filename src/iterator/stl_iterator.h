#ifndef SHADOW_STL_INTERNAL_ITERATOR_H
#define SHADOW_STL_INTERNAL_ITERATOR_H

#include "include/stl_config.h"
#include "iterator/stl_iterator_base.h"
#include "include/char_traits.h"
#include <cstddef>
#include <istream>

SHADOW_STL_BEGIN_NAMESPACE

template <typename Container>
class back_insert_iterator {
protected:
    Container* container;
public:
    using container_type = Container;
    using iterator_category = output_iterator_tag;
    using value_type = void;
    using difference_type = void;
    using pointer = void;
    using reference = void;

    explicit back_insert_iterator(Container& x) : container(&x) {}
    back_insert_iterator& operator=(const typename Container::value_type& value) {
        container->push_back(value);
        return *this;
    }
    back_insert_iterator& operator*() { return *this; }
    back_insert_iterator& operator++() { return *this; }
    back_insert_iterator& operator++(int) { return *this; }
};

template <typename Container>
inline output_iterator_tag
iterator_category(const back_insert_iterator<Container>&) {
    return output_iterator_tag();
}

template <typename Container>
inline back_insert_iterator<Container> back_inserter(Container& x) {
    return back_insert_iterator<Container>(x);
}

template <typename Container>
class front_insert_iterator {
protected:
    Container* container;
public:
    using container_type = Container;
    using iterator_category = output_iterator_tag;
    using value_type = void;
    using difference_type = void;
    using pointer = void;
    using reference = void;

    explicit front_insert_iterator(Container& x) : container(&x) {}
    front_insert_iterator& operator=(const typename Container::value_type& value) {
        container->push_front(value);
        return *this;
    }
    front_insert_iterator& operator*() { return *this; }
    front_insert_iterator& operator++() { return *this; }
    front_insert_iterator& operator++(int) { return *this; }
};

template <typename Container>
inline output_iterator_tag
iterator_category(const front_insert_iterator<Container>&) {
    return output_iterator_tag();
}

template <typename Container>
inline front_insert_iterator<Container> front_inserter(Container& x) {
    return front_insert_iterator<Container>(x);
}

template <typename Container>
class insert_iterator {
protected:
    Container* container;
    typename Container::iterator iter;
public:
    using container_type = Container;
    using iterator_category = output_iterator_tag;
    using value_type = void;
    using difference_type = void;
    using pointer = void;
    using reference = void;

    insert_iterator(Container& x, typename Container::iterator i) : container(&x), iter(i) {}
    insert_iterator& operator=(const typename Container::value_type& value) {
        iter = container->insert(iter, value);
        ++iter;
        return *this;
    }
    insert_iterator& operator*() { return *this; }
    insert_iterator& operator++() { return *this; }
    insert_iterator& operator++(int) { return *this; }
};

template <typename Container, typename Iterator>
inline output_iterator_tag
iterator_category(const insert_iterator<Container>&) {
    return output_iterator_tag();
}

template <typename Container, typename Iterator>
inline insert_iterator<Container> inserter(Container& x, Iterator i) {
    using iter = typename Container::iterator;
    return insert_iterator<Container>(x, iter(i));
}

template <typename BidirectionalIterator, typename T, typename Reference = T&, typename Distance = std::ptrdiff_t>
class reverse_bidirectional_iterator {
protected:
    BidirectionalIterator current;
public:
    using iterator_category = bidirectional_iterator_tag;
    using value_type = T;
    using difference_type = Distance;
    using pointer = T*;
    using reference = Reference;

    reverse_bidirectional_iterator() {}
    explicit reverse_bidirectional_iterator(BidirectionalIterator x) : current(x) {}
    BidirectionalIterator base() const { return current; }
    Reference operator*() const {
        BidirectionalIterator tmp = current;
        return *--tmp;
    }
    pointer operator->() const { return &(operator*()); }
    reverse_bidirectional_iterator& operator++() {
        --current;
        return *this;
    }
    reverse_bidirectional_iterator operator++(int) {
        reverse_bidirectional_iterator tmp = *this;
        --current;
        return tmp;
    }
    reverse_bidirectional_iterator& operator--() {
        ++current;
        return *this;
    }
    reverse_bidirectional_iterator operator--(int) {
        reverse_bidirectional_iterator tmp = *this;
        ++current;
        return tmp;
    }
};

template <typename Iterator, typename T, typename Reference, typename Distance>
inline bidirectional_iterator_tag
iterator_category(const reverse_bidirectional_iterator<Iterator, T, Reference, Distance>&) {
    return bidirectional_iterator_tag();
}

template <typename Iterator, typename T, typename Reference, typename Distance>
inline T*
value_type(const reverse_bidirectional_iterator<Iterator, T, Reference, Distance>&) {
    return static_cast<T*>(0);
}

template <typename Iterator, typename T, typename Reference, typename Distance>
inline Distance*
distance_type(const reverse_bidirectional_iterator<Iterator, T, Reference, Distance>&) {
    return static_cast<Distance*>(0);
}

template <typename BidirectionalIter, typename T, typename Reference, typename Distance>
inline bool operator==(
    const reverse_bidirectional_iterator<BidirectionalIter, T, Reference, Distance>& x,
    const reverse_bidirectional_iterator<BidirectionalIter, T, Reference, Distance>& y) {
    return x.base() == y.base();
}

template <typename BidirectionalIter, typename T, typename Reference, typename Distance>
inline bool operator!=(
    const reverse_bidirectional_iterator<BidirectionalIter, T, Reference, Distance>& x,
    const reverse_bidirectional_iterator<BidirectionalIter, T, Reference, Distance>& y) {
    return !(x == y);
}

// This is the new version of reverse_iterator, as defined in the
//  draft C++ standard.  It relies on the iterator_traits template,
//  which in turn relies on partial specialization.  The class
//  reverse_bidirectional_iterator is no longer part of the draft
//  standard, but it is retained for backward compatibility.
template <typename Iterator>
class reverse_iterator {
protected:
    Iterator current;
public:
    using iterator_category = typename iterator_traits<Iterator>::iterator_category;
    using value_type = typename iterator_traits<Iterator>::value_type;
    using difference_type = typename iterator_traits<Iterator>::difference_type;
    using pointer = typename iterator_traits<Iterator>::pointer;
    using reference = typename iterator_traits<Iterator>::reference;

    using iterator_type = Iterator;
    using Self = reverse_iterator<Iterator>;

    reverse_iterator() {}
    explicit reverse_iterator(iterator_type x) : current(x) {}
    reverse_iterator(const Self& x) : current(x.current) {}
    template <typename Iter>
    reverse_iterator(const reverse_iterator<Iter>& x) : current(x.current) {}

    iterator_type base() const { return current; }
    reference operator*() const {
        Iterator tmp = current;
        return *--tmp;
    }
    pointer operator->() const { return &(operator*()); }

    Self& operator++() {
        --current;
        return *this;
    }
    Self operator++(int) {
        Self tmp = *this;
        --current;
        return tmp;
    }
    Self& operator--() {
        ++current;
        return *this;
    }
    Self operator--(int) {
        Self tmp = *this;
        ++current;
        return tmp;
    }

    Self operator+(difference_type n) const {
        return Self(current - n);
    }
    Self& operator+=(difference_type n) {
        current -= n;
        return *this;
    }
    Self operator-(difference_type n) const {
        return Self(current + n);
    }
    Self& operator-=(difference_type n) {
        current += n;
        return *this;
    }
    reference operator[](difference_type n) const { return *(*this + n); }
};

template <typename Iterator>
inline bool operator==(
    const reverse_iterator<Iterator>& x,
    const reverse_iterator<Iterator>& y) {
    return x.base() == y.base();
}

template <typename Iterator>
inline bool operator<(
    const reverse_iterator<Iterator>& x,
    const reverse_iterator<Iterator>& y) {
    return y.base() < x.base();
}

template <typename Iterator>
inline bool operator!=(
    const reverse_iterator<Iterator>& x,
    const reverse_iterator<Iterator>& y) {
    return !(x == y);
}

template <typename Iterator>
inline bool operator>(
    const reverse_iterator<Iterator>& x,
    const reverse_iterator<Iterator>& y) {
    return y < x;
}

template <typename Iterator>
inline bool operator<=(
    const reverse_iterator<Iterator>& x,
    const reverse_iterator<Iterator>& y) {
    return !(y < x);
}

template <typename Iterator>
inline bool operator>=(
    const reverse_iterator<Iterator>& x,
    const reverse_iterator<Iterator>& y) {
    return !(x < y);
}

template <typename Iterator>
inline typename reverse_iterator<Iterator>::difference_type
operator-(
    const reverse_iterator<Iterator>& x,
    const reverse_iterator<Iterator>& y) {
    return y.base() - x.base();
}

template <typename Iterator>
inline reverse_iterator<Iterator>
operator+(
    typename reverse_iterator<Iterator>::difference_type n,
    const reverse_iterator<Iterator>& x) {
    return reverse_iterator<Iterator>(x.base() - n);
}

// istream_iterator and ostream_iterator look very different if we're
// using new, templatized iostreams than if we're using the old cfront
// version.
template <typename T,
        typename CharT = char, typename Traits = std::char_traits<CharT>,
        typename Dist = std::ptrdiff_t>
class istream_iterator {
public:
    using char_type = CharT;
    using traits_type = Traits;
    using istream_type = std::basic_istream<CharT, Traits>;

    using iterator_category = input_iterator_tag;
    using value_type = T;
    using difference_type = Dist;
    using pointer = T*;
    using reference = T&;

    istream_iterator() : _M_stream(0), _M_ok(false) {}
    istream_iterator(istream_type& s) : _M_stream(&s) { _M_read(); }

    reference operator*() {
        return _M_value;
    }
    pointer operator->() {
        return &(operator*());
    }

    istream_iterator& operator++() {
        _M_read();
        return *this;
    }
    istream_iterator operator++(int) {
        istream_iterator tmp = *this;
        _M_read();
        return tmp;
    }

    bool _M_equal(const istream_iterator& x) const {
        return (_M_ok == x._M_ok) && (!_M_ok || _M_stream == x._M_stream);
    }

private:
    istream_type* _M_stream;
    T _M_value;
    bool _M_ok;

    void _M_read() {
        _M_ok = (_M_stream && *_M_stream) ? true : false;
        if (_M_ok) {
            *_M_stream >> _M_value;
            _M_ok = *_M_stream ? true : false;
        }
    }
};

template <typename T, typename CharT, typename Traits, typename Dist>
inline bool operator==(
    const istream_iterator<T, CharT, Traits, Dist>& x,
    const istream_iterator<T, CharT, Traits, Dist>& y) {
    return x._M_equal(y);
}

template <typename T, typename CharT, typename Traits, typename Dist>
inline bool operator!=(
    const istream_iterator<T, CharT, Traits, Dist>& x,
    const istream_iterator<T, CharT, Traits, Dist>& y) {
    return !(x == y);
}

template <typename T,
        typename CharT = char, typename Traits = std::char_traits<CharT>>
class ostream_iterator {
public:
    using char_type = CharT;
    using traits_type = Traits;
    using ostream_type = std::basic_ostream<CharT, Traits>;

    using iterator_category = output_iterator_tag;
    using value_type = void;
    using difference_type = void;
    using pointer = void;
    using reference = void;

    ostream_iterator(ostream_type& s) : _M_stream(&s), _M_string(0) {}
    ostream_iterator(ostream_type& s, const CharT* c) : _M_stream(&s), _M_string(c) {}

    ostream_iterator& operator=(const T& value) {
        *_M_stream << value;
        if (_M_string) {
            *_M_stream << _M_string;
        }
        return *this;
    }
    ostream_iterator& operator*() { return *this; }
    ostream_iterator& operator++() { return *this; }
    ostream_iterator& operator++(int) { return *this; }
private:
    ostream_type* _M_stream;
    const CharT* _M_string;
};

// The default template argument is declared in iosfwd

// We do not read any characters until operator* is called.  The first
// time operator* is called, it calls getc.  Subsequent calls to getc 
// return a cached character, and calls to operator++ use snextc.  Before
// operator* or operator++ has been called, _M_is_initialized is false.
template <typename CharT, typename Traits>
class istreambuf_iterator : public iterator<input_iterator_tag, CharT, typename Traits::off_type, CharT*, CharT&> {
public:
    using char_type = CharT;
    using traits_type = Traits;
    using int_type = typename Traits::int_type;
    using istream_type = std::basic_istream<CharT, Traits>;
    using streambuf_type = std::basic_streambuf<CharT, Traits>;

    istreambuf_iterator(streambuf_type* p = nullptr) {
        this->_M_init(p);
    }
    istreambuf_iterator(istream_type& s) {
        this->_M_init(s.rdbuf());
    }

    char_type operator*() {
        return _M_is_initialized ? _M_c : _M_dereference_aux();
    }

    istreambuf_iterator& operator++() {
        this->_M_nextc();
        return *this;
    }
    istreambuf_iterator operator++(int) {
        if (!_M_is_initialized) {
            _M_postincr_aux();
        }
        istreambuf_iterator tmp = *this;
        this->_M_nextc();
        return tmp;
    }

    bool equal(const istreambuf_iterator& i) const {
        return this->_M_is_initialized && i._M_is_initialized 
            ? this->_M_eof == i._M_eof 
            : this->_M_equal_aux(i);
    }
private:
    streambuf_type* _M_buf;
    mutable CharT _M_c;
    mutable bool _M_eof = true;
    mutable bool _M_is_initialized = true;

    void _M_init(streambuf_type* p) {
        _M_buf = p;
        _M_eof = !p;
        _M_is_initialized = _M_eof;
    }

    char_type _M_dereference_aux() const;
    bool _M_equal_aux(const istreambuf_iterator&) const;
    void _M_postincr_aux();

    void _M_nextc() {
        int_type c = _M_buf->snextc();
        _M_c = traits_type::to_char_type(c);
        _M_eof = traits_type::eq_int_type(c, traits_type::eof());
        _M_is_initialized = true;
    }

    void _M_getc() const {
        int_type c = _M_buf->sgetc();
        _M_c = traits_type::to_char_type(c);
        _M_eof = traits_type::eq_int_type(c, traits_type::eof());
        _M_is_initialized = true;
    }
};

template <typename CharT, typename Traits>
CharT istreambuf_iterator<CharT, Traits>::_M_dereference_aux() const {
    this->_M_getc();
    return _M_c;
}

template <typename CharT, typename Traits>
bool istreambuf_iterator<CharT, Traits>::_M_equal_aux(const istreambuf_iterator& i) const {
    if (!this->_M_is_initialized) {
        this->_M_getc();
    }
    if (!i._M_is_initialized) {
        i._M_getc();
    }
    return this->_M_eof == i._M_eof;
}

template <typename CharT, typename Traits>
void istreambuf_iterator<CharT, Traits>::_M_postincr_aux() {
    this->_M_getc();
}

template <typename CharT, typename Traits>
inline bool operator==(
    const istreambuf_iterator<CharT, Traits>& x,
    const istreambuf_iterator<CharT, Traits>& y) {
    return x.equal(y);
}

template <typename CharT, typename Traits>
inline bool operator!=(
    const istreambuf_iterator<CharT, Traits>& x,
    const istreambuf_iterator<CharT, Traits>& y) {
    return !(x == y);
}

// The default template argument is declared in iosfwd
template <typename CharT, typename Traits>
class ostreambuf_iterator : public iterator<output_iterator_tag, void, void, void, void> {
public:
    using char_type = CharT;
    using traits_type = Traits;
    using ostream_type = std::basic_ostream<CharT, Traits>;
    using streambuf_type = std::basic_streambuf<CharT, Traits>;

    ostreambuf_iterator(ostream_type& s) : _M_buf(s.rdbuf()), _M_ok(s.rdbuf() != nullptr) {}
    ostreambuf_iterator(streambuf_type* b) : _M_buf(b), _M_ok(b) {}

    ostreambuf_iterator& operator=(char_type c) {
        _M_ok = _M_ok && !traits_type::eq_int_type(_M_buf->sputc(c), traits_type::eof());
    }

    ostreambuf_iterator& operator*() { return *this; }
    ostreambuf_iterator& operator++() { return *this; }
    ostreambuf_iterator operator++(int) { return *this; }

    bool failed() const { return !_M_ok; }
private:
    streambuf_type* _M_buf;
    bool _M_ok;
};


SHADOW_STL_END_NAMESPACE

#endif // SHADOW_STL_INTERNAL_ITERATOR_H