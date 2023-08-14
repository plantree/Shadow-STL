#include <thread>
#include <iostream>

#include <catch2/catch_test_macros.hpp>
#include "allocator/stl_alloc.h"
#include "allocator/stl_construct.h"
#include "allocator/stl_unitialized.h"

SHADOW_STL_BEGIN_NAMESPACE

class T {
public:
    T(int t) : _t(t) {
        std::cout << "T()" << std::endl;
    }
    T(const T& t) : _t(t._t) {
        std::cout << "T(const T&)" << std::endl;
    }
    ~T() {
        std::cout << "~T()" << std::endl;
    }
    int value() const {
        return _t;
    }
private:
    int _t = 0;
};

TEST_CASE("Normal Construct", "[stl_construct]") {
    allocator<int> alloc;
    int* p = alloc.allocate(1);
    REQUIRE(p != nullptr);
    construct(p, 10);
    REQUIRE(*p == 10);
    destroy(p);
    alloc.deallocate(p, 1);
}

TEST_CASE("Class Construct", "[stl_construct]") {
    allocator<T> alloc;
    T* p = alloc.allocate(1);
    REQUIRE(p != nullptr);
    T t(1);
    construct(p, t);
    REQUIRE(p->value() == 1);
    destroy(p);
    alloc.deallocate(p, 1);
}

TEST_CASE("Unitialized copy", "[stl_unitialized]") {
    allocator<T> alloc;
    T* p = alloc.allocate(1);
    REQUIRE(p != nullptr);
    T t(1);
    uninitialized_copy(&t, &t + 1, p);
    REQUIRE(p->value() == 1);
    destroy(p);
    alloc.deallocate(p, 1);
}

TEST_CASE("Unitialized fill", "[stl_unitialized]") {
    allocator<T> alloc;
    T* p = alloc.allocate(1);
    REQUIRE(p != nullptr);
    T t(1);
    uninitialized_fill(p, p + 1, t);
    REQUIRE(p->value() == 1);
    destroy(p);
    alloc.deallocate(p, 1);
}

SHADOW_STL_END_NAMESPACE