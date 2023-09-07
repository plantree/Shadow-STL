#include "container/slist.h"
#include <catch2/catch_test_macros.hpp>

SHADOW_STL_BEGIN_NAMESPACE

TEST_CASE("slist", "[stl_slist]") {
  slist<int> l;
  REQUIRE(l.empty());
  REQUIRE(l.size() == 0);

  l.push_front(1);
  REQUIRE(l.size() == 1);
  REQUIRE(l.front() == 1);

  l.push_front(2);
  REQUIRE(l.size() == 2);
  REQUIRE(l.front() == 2);

  l.pop_front();
  REQUIRE(l.size() == 1);
  REQUIRE(l.front() == 1);
}

SHADOW_STL_END_NAMESPACE