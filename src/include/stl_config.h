#ifndef SHADOWS_STL_CONFIG_H
#define SHADOWS_STL_CONFIG_H

// Flags:  常量定义的解释
// * SHADOW_STL_NO_BOOL: defined if the compiler doesn't have bool as a builtin
//   type.
// * SHADOW_STL_HAS_WCHAR_T: defined if the compier has wchar_t as a builtin type.
// * SHADOW_STL_NO_DRAND48: defined if the compiler doesn't have the drand48 
//   function.
// * SHADOW_STL_STATIC_TEMPLATE_MEMBER_BUG: defined if the compiler can't handle
//   static members of template classes.
// * SHADOW_STL_STATIC_CONST_INIT_BUG: defined if the compiler can't handle a
//   constant-initializer in the declaration of a static const data member
//   of integer type.  (See section 9.4.2, paragraph 4, of the C++ standard.)
// * SHADOW_STL_CLASS_PARTIAL_SPECIALIZATION: defined if the compiler supports
//   partial specialization of template classes.
// * SHADOW_STL_PARTIAL_SPECIALIZATION_SYNTAX: defined if the compiler 
//   supports partial specialization syntax for full specialization of
//   class templates.  (Even if it doesn't actually support partial 
//   specialization itself.)
// * SHADOW_STL_FUNCTION_TMPL_PARTIAL_ORDER: defined if the compiler supports
//   partial ordering of function templates.  (a.k.a partial specialization
//   of function templates.)
// * SHADOW_STL_MEMBER_TEMPLATES: defined if the compiler supports template
//   member functions of classes.
// * SHADOW_STL_MEMBER_TEMPLATE_CLASSES: defined if the compiler supports 
//   nested classes that are member templates of other classes.
// * SHADOW_STL_TEMPLATE_FRIENDS: defined if the compiler supports templatized
//   friend declarations.
// * SHADOW_STL_EXPLICIT_FUNCTION_TMPL_ARGS: defined if the compiler 
//   supports calling a function template by providing its template
//   arguments explicitly.
// * SHADOW_STL_LIMITED_DEFAULT_TEMPLATES: defined if the compiler is unable
//   to handle default template parameters that depend on previous template
//   parameters.
// * SHADOW_STL_NON_TYPE_TMPL_PARAM_BUG: defined if the compiler has trouble with
//   function template argument deduction for non-type template parameters.
// * __SGI_STL_NO_ARROW_OPERATOR: defined if the compiler is unable
//   to support the -> operator for iterators.
// * SHADOW_STL_DEFAULT_CONSTRUCTOR_BUG: defined if T() does not work properly
//   when T is a builtin type.
// * SHADOW_STL_USE_EXCEPTIONS: defined if the compiler (in the current compilation
//   mode) supports exceptions.
// * SHADOW_STL_USE_NAMESPACES: defined if the compiler has the necessary
//   support for namespaces.
// * SHADOW_STL_NO_EXCEPTION_HEADER: defined if the compiler does not have a
//   standard-conforming header <exception>.
// * SHADOW_STL_NO_BAD_ALLOC: defined if the compiler does not have a <new>
//   header, or if <new> does not contain a bad_alloc class.  If a bad_alloc
//   class exists, it is assumed to be in namespace std.
// * SHADOW_STL_SGI_THREADS: defined if this is being compiled for an SGI IRIX
//   system in multithreaded mode, using native SGI threads instead of 
//   pthreads.
// * SHADOW_STL_WIN32THREADS: defined if this is being compiled on a WIN32
//   compiler in multithreaded mode.
// * SHADOW_STL_PTHREADS: defined if we should use portable pthreads
//   synchronization.
// * SHADOW_STL_UITHREADS: defined if we should use UI / solaris / UnixWare threads
//   synchronization.  UIthreads are similar to pthreads, but are based 
//   on an earlier version of the Posix threads standard.
// * SHADOW_STL_LONG_LONG if the compiler has long long and unsigned long long
//   types.  (They're not in the C++ standard, but they are expected to be 
//   included in the forthcoming C9X standard.)
// * SHADOW_STL_THREADS is defined if thread safety is needed.
// * SHADOW_STL_VOLATILE is defined to be "volatile" if threads are being
//   used, and the empty string otherwise.
// * SHADOW_STL_USE_CONCEPT_CHECKS enables some extra compile-time error
//   checking to make sure that user-defined template arguments satisfy
//   all of the appropriate requirements.  This may result in more
//   comprehensible error messages.  It incurs no runtime overhead.  This 
//   feature requires member templates and partial specialization.
// * SHADOW_STL_NO_USING_CLAUSE_IN_CLASS: The compiler does not handle "using"
//   clauses inside of class definitions.
// * SHADOW_STL_NO_FRIEND_TEMPLATE_CLASS: The compiler does not handle friend
//   declaractions where the friend is a template class.
// * SHADOW_STL_NO_FUNCTION_PTR_IN_CLASS_TEMPLATE: The compiler does not
//   support the use of a function pointer type as the argument
//   for a template.
// * SHADOW_STL_MEMBER_TEMPLATE_KEYWORD: standard C++ requires the template
//   keyword in a few new places (14.2.4).  This flag is set for
//   compilers that support (and require) this usage.

// User-settable macros that control compilation:
// * SHADOW_STL_USE_SGI_ALLOCATORS: if defined, then the STL will use older
//   SGI-style allocators, instead of standard-conforming allocators,
//   even if the compiler supports all of the language features needed
//   for standard-conforming allocators.
// * SHADOW_STL_NO_NAMESPACES: if defined, don't put the library in namespace
//   std, even if the compiler supports namespaces.
// * SHADOW_STL_NO_RELOPS_NAMESPACE: if defined, don't put the relational
//   operator templates (>, <=. >=, !=) in namespace std::rel_ops, even
//   if the compiler supports namespaces and partial ordering of
//   function templates.
// * SHADOW_STL_ASSERTIONS: if defined, then enable runtime checking through the
//   SHADOW_STL_assert macro.
// * _PTHREADS: if defined, use Posix threads for multithreading support.
// * _UITHREADS:if defined, use SCO/Solaris/UI threads for multithreading 
//   support
// * _NOTHREADS: if defined, don't use any multithreading support.  
// * _STL_NO_CONCEPT_CHECKS: if defined, disables the error checking that
//   we get from SHADOW_STL_USE_CONCEPT_CHECKS.
// * SHADOW_STL_USE_NEW_IOSTREAMS: if defined, then the STL will use new,
//   standard-conforming iostreams (e.g. the <iosfwd> header).  If not
//   defined, the STL will use old cfront-style iostreams (e.g. the
//   <iostream.h> header).

// Other macros defined by this file:

// * bool, true, and false, if SHADOW_STL_NO_BOOL is defined.
// * typename, as a null macro if it's not already a keyword.
// * explicit, as a null macro if it's not already a keyword.
// * namespace-related macros (SHADOW_STD, SHADOW_STL_BEGIN_NAMESPACE, etc.)
// * exception-related macros (SHADOW_STL_TRY, SHADOW_STL_UNWIND, etc.)
// * SHADOW_STL_assert, either as a test or as a null macro, depending on
//   whether or not SHADOW_STL_ASSERTIONS is defined.


// SHADOW_STL_NO_NAMESPACES is a hook so that users can disable namespaces
// without having to edit library headers.  SHADOW_STL_NO_RELOPS_NAMESPACE is
// a hook so that users can disable the std::rel_ops namespace, keeping 
// the relational operator template in namespace std, without having to 
// edit library headers.
#define SHADOW_STD 
#define SHADOW_STL_BEGIN_NAMESPACE 
#define SHADOW_STL_END_NAMESPACE 
#undef  SHADOW_STL_USE_NAMESPACE_FOR_RELOPS
#define SHADOW_STL_BEGIN_RELOPS_NAMESPACE 
#define SHADOW_STL_END_RELOPS_NAMESPACE 
#define SHADOW_STD_RELOPS 
#undef  SHADOW_STL_USE_NAMESPACES
#endif // SHADOW_STL_NO_NAMESPACES

// 下面是 GNU C++ 编译器支持
# ifdef __GNUC__
#if __GNUC__ == 2 && __GNUC_MINOR__ <= 7
#define SHADOW_STL_STATIC_TEMPLATE_MEMBER_BUG    /* 模板类支持静态数据成员初始化 */
#endif // __GNUC__ == 2 && __GNUC_MINOR__ <= 7

#if __GNUC__ < 2 
#define SHADOW_STL_NEED_TYPENAME
#define SHADOW_STL_NEED_EXPLICIT
#endif // __GNUC__ < 2

#if __GNUC__ == 2 && __GNUC_MINOR__ <= 8
#define SHADOW_STL_NO_EXCEPTION_HEADER
#define SHADOW_STL_NO_BAD_ALLOC
#endif // __GNUC__ == 2 && __GNUC_MINOR__ <= 8

#if __GNUC__ == 2 && __GNUC_MINOR__ >= 8
#define SHADOW_STL_CLASS_PARTIAL_SPECIALIZATION   /* 针对对模板参数做部分特化 */
#define SHADOW_STL_FUNCTION_TMPL_PARTIAL_ORDER    /* 针对对函数模板做部分特化 */
#define SHADOW_STL_EXPLICIT_FUNCTION_TMPL_ARGS    /* 调用一个 function template 时可以明白指定其 template arguments */
#define SHADOW_STL_MEMBER_TEMPLATES               /* 类模板中嵌套成员模板 */
#define SHADOW_STL_CAN_THROW_RANGE_ERRORS
// g++ 2.8.1 supports member template functions, but not member
// template nested classes.

#if __GNUC_MINOR__ >= 9
#define SHADOW_STL_MEMBER_TEMPLATE_CLASSES
#define SHADOW_STL_TEMPLATE_FRIENDS
#define __SGI_STL_USE_AUTO_PTR_CONVERSIONS
#define SHADOW_STL_HAS_NAMESPACES
//#define SHADOW_STL_USE_NEW_IOSTREAMS
#endif // __GNUC_MINOR__ >= 9

#endif // __GNUC__ == 2 && __GNUC_MINOR__ >= 8

#define SHADOW_STL_DEFAULT_CONSTRUCTOR_BUG

#define SHADOW_STL_USE_EXCEPTIONS

#define SHADOW_STL_PTHREADS

#if (__GNUC__ < 2) || (__GNUC__ == 2 && __GNUC_MINOR__ < 95)
#define SHADOW_STL_NO_FUNCTION_PTR_IN_CLASS_TEMPLATE
#endif  // __GNUC__ < 2 || __GNUC__ == 2 && __GNUC_MINOR__ < 95

#endif  // SHADOW_STL_CONFIG_H