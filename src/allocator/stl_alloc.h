#ifndef SHADOW_STL_INTERNAL_ALLOC_H
#define SHADOW_STL_INTERNAL_ALLOC_H

// <stl_alloc.h> 被包含到其它的 STL 头文件中，并不是直接使用
// SGI STL 两级配置

#ifdef SHADOW_STL_STATIC_TEMPLATE_MEMBER_BUG
#define USE_MALLOC
#endif // SHADOW_STL_STATIC_TEMPLATE_MEMBER_BUG

// This implements some standard node allocators.  These are
// NOT the same as the allocators in the C++ draft standard or in
// in the original STL.  They do not encapsulate different pointer
// types; indeed we assume that there is only one pointer type.
// The allocation primitives are intended to allocate individual objects,
// not larger arenas as with the original STL allocators.

#ifndef SHADOW_THROW_BAD_ALLOC
#if defined(SHADOW_STL_NO_BAD_ALLOC) || !defined(SHADOW_STL_USE_EXCEPTIONS)
#include <stdio.h>
#include <stdlib.h>
#define SHADOW_THROW_BAD_ALLOC fprintf(stderr, "out of memory\n"); exit(1)
#else /* Standard conforming out-of-memory handling */
#include <new>
#define SHADOW_THROW_BAD_ALLOC throw std::bad_alloc()
#endif 

#endif // SHADOW_STL_NO_BAD_ALLOC

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "include/stl_config.h"

#ifndef SHADOW_RESTRICT
#define SHADOW_RESTRICT
#endif // SHADOW_RESTRICT

#ifdef SHADOW_STL_THREADS
#include "stl_threads.h"
#define SHADOW_NODE_ALLOCATOR_THREADS true
#define SHADOW_NODE_ALLOCATOR_LOCK if (threads) \
    { _S_node_allocator_lock._M_acquire_lock(); }
#define SHADOW_NODE_ALLOCATOR_UNLOCK if (threads) \
    { _S_node_allocator_lock._M_release_lock(); }
#else   
// Thread-unsafe
#define SHADOW_NODE_ALLOCATOR_THREADS false
#define SHADOW_NODE_ALLOCATOR_LOCK
#define SHADOW_NODE_ALLOCATOR_UNLOCK



#endif // SHADOW_STL_THREADS







#endif // SHADOW_STL_INTERNAL_ALLOC_H