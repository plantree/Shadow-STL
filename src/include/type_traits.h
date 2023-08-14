#ifndef SHADOW_TYPE_TRAITS_H
#define SHADOW_TYPE_TRAITS_H

#ifndef SHADOW_STL_CONFIG_H
#include "include/stl_config.h"
#endif  // SHADOW_STL_CONFIG_H

/*
This header file provides a framework for allowing compile time dispatch
based on type attributes. This is useful when writing template code.
For example, when making a copy of an array of an unknown type, it helps
to know if the type has a trivial copy constructor or not, to help decide
if a memcpy can be used.
The class template __type_traits provides a series of typedefs each of
which is either __true_type or __false_type. The argument to
__type_traits can be any type. The typedefs within this template will
attain their correct values by one of these means:
    1. The general instantiation contain conservative values which work
       for all types.
    2. Specializations may be declared to make distinctions between types.
    3. Some compilers (such as the Silicon Graphics N32 and N64 compilers)
       will automatically provide the appropriate specializations for all
       types.
EXAMPLE:
//Copy an array of elements which have non-trivial copy constructors
template <class T> void copy(T* source, T* destination, int n, __false_type);
//Copy an array of elements which have trivial copy constructors. Use memcpy.
template <class T> void copy(T* source, T* destination, int n, __true_type);
//Copy an array of any type by using the most efficient copy mechanism
template <class T> inline void copy(T* source,T* destination,int n) {
   copy(source, destination, n,
        typename __type_traits<T>::has_trivial_copy_constructor());
}
*/

struct _true_type {};

struct _false_type {};

template <typename T>
struct _type_traits {
    using this_dummy_member_must_be_first = _true_type;
                       /* Do not remove this member. It informs a compiler which
                      automatically specializes __type_traits that this
                      __type_traits template is special. It just makes sure that
                      things work if an implementation is using a template
                      called __type_traits for something unrelated. */

   /* The following restrictions should be observed for the sake of
      compilers which automatically produce type specific specializations 
      of this class:
          - You may reorder the members below if you wish
          - You may remove any of the members below if you wish
          - You must not rename members without making the corresponding
            name change in the compiler
          - Members you add will be treated like regular members unless
            you add the appropriate support in the compiler. */
    using has_trivial_default_constructor = _false_type;
    using has_trivial_copy_constructor = _false_type;
    using has_trivial_assignment_operator = _false_type;
    using has_trivial_destructor = _false_type;
    using is_POD_type = _false_type;
};

// Provide some specializations.  This is harmless for compilers that
//  have built-in __types_traits support, and essential for compilers
//  that don't.
template <>
struct _type_traits<bool> {
    using has_trivial_default_constructor = _true_type;
    using has_trivial_copy_constructor = _true_type;
    using has_trivial_assignment_operator = _true_type;
    using has_trivial_destructor = _true_type;
    using is_POD_type = _true_type;
};

template<>
struct _type_traits<char> {
    using has_trivial_default_constructor = _true_type;
    using has_trivial_copy_constructor = _true_type;
    using has_trivial_assignment_operator = _true_type;
    using has_trivial_destructor = _true_type;
    using is_POD_type = _true_type;
};

template<>
struct _type_traits<signed char> {
    using has_trivial_default_constructor = _true_type;
    using has_trivial_copy_constructor = _true_type;
    using has_trivial_assignment_operator = _true_type;
    using has_trivial_destructor = _true_type;
    using is_POD_type = _true_type;
};

template<>
struct _type_traits<unsigned char> {
    using has_trivial_default_constructor = _true_type;
    using has_trivial_copy_constructor = _true_type;
    using has_trivial_assignment_operator = _true_type;
    using has_trivial_destructor = _true_type;
    using is_POD_type = _true_type;
};

template<>
struct _type_traits<wchar_t> {
    using has_trivial_default_constructor = _true_type;
    using has_trivial_copy_constructor = _true_type;
    using has_trivial_assignment_operator = _true_type;
    using has_trivial_destructor = _true_type;
    using is_POD_type = _true_type;
};

template<>
struct _type_traits<short> {
    using has_trivial_default_constructor = _true_type;
    using has_trivial_copy_constructor = _true_type;
    using has_trivial_assignment_operator = _true_type;
    using has_trivial_destructor = _true_type;
    using is_POD_type = _true_type;
};

template<>
struct _type_traits<unsigned short> {
    using has_trivial_default_constructor = _true_type;
    using has_trivial_copy_constructor = _true_type;
    using has_trivial_assignment_operator = _true_type;
    using has_trivial_destructor = _true_type;
    using is_POD_type = _true_type;
};

template<>
struct _type_traits<int> {
    using has_trivial_default_constructor = _true_type;
    using has_trivial_copy_constructor = _true_type;
    using has_trivial_assignment_operator = _true_type; 
    using has_trivial_destructor = _true_type;
    using is_POD_type = _true_type;
};

template<>
struct _type_traits<unsigned int> {
    using has_trivial_default_constructor = _true_type;
    using has_trivial_copy_constructor = _true_type; 
    using has_trivial_assignment_operator = _true_type; 
    using has_trivial_destructor = _true_type;
    using is_POD_type = _true_type;
};

template<>
struct _type_traits<long> {
    using has_trivial_default_constructor = _true_type;
    using has_trivial_copy_constructor = _true_type; 
    using has_trivial_assignment_operator = _true_type; 
    using has_trivial_destructor = _true_type;
    using is_POD_type = _true_type;
};

template<>
struct _type_traits<unsigned long> {
    using has_trivial_default_constructor = _true_type;
    using has_trivial_copy_constructor = _true_type; 
    using has_trivial_assignment_operator = _true_type; 
    using has_trivial_destructor = _true_type;
    using is_POD_type = _true_type;
};

template<>
struct _type_traits<long long> {
    using has_trivial_default_constructor = _true_type;
    using has_trivial_copy_constructor = _true_type; 
    using has_trivial_assignment_operator = _true_type; 
    using has_trivial_destructor = _true_type;
    using is_POD_type = _true_type;
};

template<>
struct _type_traits<unsigned long long> {
    using has_trivial_default_constructor = _true_type;
    using has_trivial_copy_constructor = _true_type; 
    using has_trivial_assignment_operator = _true_type; 
    using has_trivial_destructor = _true_type;
    using is_POD_type = _true_type;
};

template<>
struct _type_traits<float> {
    using has_trivial_default_constructor = _true_type;
    using has_trivial_copy_constructor = _true_type; 
    using has_trivial_assignment_operator = _true_type; 
    using has_trivial_destructor = _true_type;
    using is_POD_type = _true_type;
};

template<>
struct _type_traits<double> {
    using has_trivial_default_constructor = _true_type;
    using has_trivial_copy_constructor = _true_type; 
    using has_trivial_assignment_operator = _true_type; 
    using has_trivial_destructor = _true_type;
    using is_POD_type = _true_type;
};

template<>
struct _type_traits<long double> {
    using has_trivial_default_constructor = _true_type;
    using has_trivial_copy_constructor = _true_type; 
    using has_trivial_assignment_operator = _true_type; 
    using has_trivial_destructor = _true_type;
    using is_POD_type = _true_type;
};

template <typename T>
struct _type_traits<T*> {
    using has_trivial_default_constructor = _true_type;
    using has_trivial_copy_constructor = _true_type;
    using has_trivial_assignment_operator = _true_type;
    using has_trivial_destructor = _true_type;
    using is_POD_type = _true_type; 
};

template<>
struct _type_traits<char*> {
    using has_trivial_default_constructor = _true_type;
    using has_trivial_copy_constructor = _true_type;
    using has_trivial_assignment_operator = _true_type;
    using has_trivial_destructor = _true_type;
    using is_POD_type = _true_type; 
};

template<>
struct _type_traits<signed char*> {
    using has_trivial_default_constructor = _true_type;
    using has_trivial_copy_constructor = _true_type;
    using has_trivial_assignment_operator = _true_type;
    using has_trivial_destructor = _true_type;
    using is_POD_type = _true_type; 
};

template<>
struct _type_traits<unsigned char*> {
    using has_trivial_default_constructor = _true_type;
    using has_trivial_copy_constructor = _true_type;
    using has_trivial_assignment_operator = _true_type;
    using has_trivial_destructor = _true_type;
    using is_POD_type = _true_type; 
};

template<>
struct _type_traits<const char*> {
    using has_trivial_default_constructor = _true_type;
    using has_trivial_copy_constructor = _true_type;
    using has_trivial_assignment_operator = _true_type;
    using has_trivial_destructor = _true_type;
    using is_POD_type = _true_type; 
};

template<>
struct _type_traits<const signed char*> {
    using has_trivial_default_constructor = _true_type;
    using has_trivial_copy_constructor = _true_type;
    using has_trivial_assignment_operator = _true_type;
    using has_trivial_destructor = _true_type;
    using is_POD_type = _true_type; 
};

template<>
struct _type_traits<const unsigned char*> {
    using has_trivial_default_constructor = _true_type;
    using has_trivial_copy_constructor = _true_type;
    using has_trivial_assignment_operator = _true_type;
    using has_trivial_destructor = _true_type;
    using is_POD_type = _true_type; 
};

// The following could be written in terms of numeric_limits.  
// We're doing it separately to reduce the number of dependencies.
template <typename T> 
struct _Is_integer {
    using _Integral = _false_type;
};

template<> 
struct _Is_integer<bool> {
    using _Integral = _true_type;
};

template<>
struct _Is_integer<char> {
    using _Integral = _true_type;
};

template<>
struct _Is_integer<signed char> {
    using _Integral = _true_type;
};

template<>
struct _Is_integer<unsigned char> {
    using _Integral = _true_type;
};

template<>
struct _Is_integer<wchar_t> {
    using _Integral = _true_type;
};

template<>
struct _Is_integer<short> {
    using _Integral = _true_type;
};

template<>
struct _Is_integer<unsigned short> {
    using _Integral = _true_type;
};

template<>
struct _Is_integer<int> {
    using _Integral = _true_type;
};

template<>
struct _Is_integer<unsigned int> {
    using _Integral = _true_type;
};


template<>
struct _Is_integer<long> {
    using _Integral = _true_type;
};

template<>
struct _Is_integer<unsigned long> {
    using _Integral = _true_type;
};

template<>
struct _Is_integer<long long> {
    using _Integral = _true_type;
};

template<>
struct _Is_integer<unsigned long long> {
    using _Integral = _true_type;
};

#endif // SHADOW_TYPE_TRAITS_H