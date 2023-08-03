#ifndef SHADOW_STL_ALLOC_H
#define SHADOW_STL_ALLOC_H

#ifndef SHADOW_STL_CONFIG_H
#include "stl_config.h"
#endif

#ifndef SHADOW_STL_INTERNAL_ALLOC_H
#include "allocator/stl_alloc.h"
#endif

#ifdef SHADOW_STL_NAMESPACES

using SHADOW_STD::malloc_alloc_template; 
using SHADOW_STD::malloc_alloc; 
using SHADOW_STD::simple_alloc; 
using SHADOW_STD::debug_alloc; 
using SHADOW_STD::default_alloc_template; 
using SHADOW_STD::alloc; 
using SHADOW_STD::single_client_alloc; 

#ifdef SHADOW_STL_STATIC_TEMPLATE_MEMBER_BUG
using SHADOW_STD::malloc_alloc_oom_handler;
#endif // SHADOW_STL_STATIC_TEMPLATE_MEMBER_BUG

#ifdef SHADOW_STL_USE_STD_ALLOCATORS
using SHADOW_STD::allocator;
#endif // SHADOW_STL_USE_STD_ALLOCATORS

#endif // SHADOW_STL_NAMESPACES

#endif  // SHADOW_STL_ALLOC_H