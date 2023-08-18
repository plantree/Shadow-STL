#include <thread>
#include <iostream>
#include <sstream>

#include <catch2/catch_test_macros.hpp>
#include "iterator/iterator.h"

SHADOW_STL_BEGIN_NAMESPACE

TEST_CASE("auto ptr", "[stl_iterator]") {
    int* p = new int(1);
    auto_ptr<int> ap(p);
    REQUIRE(*ap == 1);
    REQUIRE(ap.get() == p);
    REQUIRE(ap.release() == p);
    REQUIRE(ap.get() == nullptr);
    ap.reset(p);
    REQUIRE(*ap == 1);
    REQUIRE(ap.get() == p);
}

TEST_CASE("istream iterator", "[stl_iterator]") {
    std::istringstream iss("1 2 3 4 5");
    istream_iterator<int> isi(iss);
    REQUIRE(*isi == 1);
    REQUIRE(*++isi == 2);
    REQUIRE(*++isi == 3);
    REQUIRE(*++isi == 4);
    REQUIRE(*++isi == 5);
    REQUIRE(++isi == istream_iterator<int>());
}

TEST_CASE("ostream iterator", "[stl_iterator]") {
    std::ostringstream oss;
    ostream_iterator<int> osi(oss, " ");
    *osi++ = 1;
    *osi++ = 2;
    *osi++ = 3;
    *osi++ = 4;
    *osi++ = 5;
    REQUIRE(oss.str() == "1 2 3 4 5 ");
}

SHADOW_STL_END_NAMESPACE