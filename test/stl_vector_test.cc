#include <catch2/catch_test_macros.hpp>
#include "container/vector.h"

SHADOW_STL_BEGIN_NAMESPACE

TEST_CASE("vector", "[stl_vector]") {
    vector<int> v;
    REQUIRE(v.empty());
    REQUIRE(v.size() == 0);
    REQUIRE(v.capacity() == 0);

    v.push_back(1);
    REQUIRE(v.size() == 1);
    REQUIRE(v.capacity() == 1);
    REQUIRE(v[0] == 1);
    v.push_back(2);
    REQUIRE(v.size() == 2);
    REQUIRE(v.capacity() == 2);

    v.push_back(3);
    REQUIRE(v.size() == 3);
    REQUIRE(v.capacity() == 4);
}

SHADOW_STL_END_NAMESPACE