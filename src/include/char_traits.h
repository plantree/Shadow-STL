#ifndef SHADOW_STL_CHAR_TRAITS_H
#define SHADOW_STL_CHAR_TRAITS_H

#include "include/stl_config.h"
#include <bits/types/mbstate_t.h>
#include <cstddef>
#include <cstring>
#include <cwchar>
#include <ios>
#include <iosfwd>

SHADOW_STL_BEGIN_NAMESPACE

// Class char_traits_base
template <typename CharT, typename IntT>
class _char_traits_base {
public:
    using char_type = CharT;
    using int_type = IntT;
    using off_type = std::streamoff;
    using pos_type = std::streampos;
    using state_type = mbstate_t;

    static void assign(char_type& c1, const char_type& c2) {
        c1 = c2;
    }
    static bool eq(const char_type& c1, const char_type& c2) {
        return c1 == c2;
    }
    static bool lt(const char_type& c1, const char_type& c2) {
        return c1 < c2;
    }

    static int compare(const char_type* s1, const char_type* s2, size_t n) {
        for (size_t i = 0; i < n; ++i) {
            if (!eq(s1[i], s2[i])) {
                return s1[i] < s2[i] ? -1 : 1;
            }
        }
        return 0;
    }

    static size_t length(const char_type* s) {
        const char_type nullchar = char_type();
        size_t i = 0;
        for (; !eq(s[i], nullchar); ++i) {}
        return i;
    }

    static char_type* find(const char_type* s, size_t n, const char_type& c) {
        for(; n > 0; ++s, --n) {
            if (eq(*s, c)) {
                return s;
            }
        }
        return nullptr;
    }

    static char_type* move(char_type* s1, const char_type* s2, size_t n) {
        memmove(s1, s2, n * sizeof(char_type));
        return s1;
    }

    static char_type* copy(char_type* s1, const char_type* s2, size_t n) {
        memcpy(s1, s2, n * sizeof(char_type));
        return s1;
    }

    static char_type* assign(char_type* s, size_t n, char_type c) {
        for (size_t i = 0; i < n; ++i) {
            s[i] = c;
        }
        return s;
    }

    static int_type eof() {
        return static_cast<int_type>(-1);
    }

    static bool eq_int_type(const int_type& c1, const int_type& c2) {
        return c1 == c2;
    }

    static int_type not_eof(const int_type& c) {
        return !eq_int_type(c, eof()) ? c : 0;
    }

    static char_type to_char_type(const int_type& c) {
        return static_cast<char_type>(c);
    }

    static int_type to_int_type(const char_type& c) {
        return static_cast<int_type>(c);
    }
};

// Generic char_traits class.  Note that this class is provided only
//  as a base for explicit specialization; it is unlikely to be useful
//  as is for any particular user-defined type.  In particular, it 
//  *will not work* for a non-POD type.
template <typename CharT>
class char_traits : public _char_traits_base<CharT, CharT> {};

// Specialization for char.
template <>
class char_traits<char> : public _char_traits_base<char, int> {
public:
    static char_type to_char_type(const int_type& c) {
        return static_cast<char_type>(static_cast<unsigned char>(c));
    }

    static int_type to_int_type(const char_type& c) {
        return static_cast<unsigned char>(c);
    }

    static int compare(const char* s1, const char* s2, size_t n) {
        return memcmp(s1, s2, n);
    }

    static size_t length(const char* s) {
        return strlen(s);
    }

    static void assign(char& c1, const char& c2) {
        c1 = c2;
    }

    static char* assign(char* s, size_t n, char c) {
        memset(s, c, n);
        return s;
    }
};

// Specialization for wchar_t.
template <>
class char_traits<wchar_t> : public _char_traits_base<wchar_t, wchar_t> {};

SHADOW_STL_END_NAMESPACE

#endif // SHADOW_STL_CHAR_TRAITS_H