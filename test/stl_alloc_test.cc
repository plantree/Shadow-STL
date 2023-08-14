#include <thread>

#include <catch2/catch_test_macros.hpp>
#include "allocator/stl_alloc.h"
#include "allocator/stl_unitialized.h"

SHADOW_STL_BEGIN_NAMESPACE

TEST_CASE("Allocator", "[stl_alloc]") {
    allocator<int> alloc;
    int* p = alloc.allocate(100);
    REQUIRE(p != nullptr);
    alloc.deallocate(p, 100);
}

SHADOW_STL_END_NAMESPACE