#include "container/list.h"
#include <catch2/catch_test_macros.hpp>

SHADOW_STL_BEGIN_NAMESPACE

TEST_CASE("list", "[stl_list]") {
  list<int> l;
  REQUIRE(l.empty());
  REQUIRE(l.size() == 0);

  l.push_back(1);
  REQUIRE(l.size() == 1);
  REQUIRE(l.front() == 1);

  l.push_front(2);
  REQUIRE(l.size() == 2);
  REQUIRE(l.front() == 2);
  REQUIRE(l.back() == 1);

  l.pop_back();
  REQUIRE(l.size() == 1);
  REQUIRE(l.front() == 2);
  REQUIRE(l.back() == 2);
}

SHADOW_STL_END_NAMESPACE