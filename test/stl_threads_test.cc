#include <thread>

#include <catch2/catch_test_macros.hpp>
#include "include/stl_threads.h"

SHADOW_STL_BEGIN_NAMESPACE

TEST_CASE("Refcount_Base", "[stl_threads]") {
    // multi-thread share the same _Refcount_Base object
    _Refcount_Base rb(0);
    std::thread t1([&rb]() {
        // sleep 1 second
        std::this_thread::sleep_for(std::chrono::seconds(1));
        rb._M_incr();
        REQUIRE(rb._M_ref_count == 1);
        rb._M_decr();
        REQUIRE(rb._M_ref_count == 0);
        rb._M_incr();
    });
    REQUIRE(rb._M_ref_count == 0);
    t1.join();
    REQUIRE(rb._M_ref_count == 1);
}

TEST_CASE("Atomic_swap", "[stl_threads]") {
    unsigned long p = 3;
    unsigned long q = 4;
    std::thread t1([&p, &q]() {
        unsigned long tmp = _Atomic_swap(&p, q);
        REQUIRE(tmp == 3);
        REQUIRE(p == 4);
    });
    t1.join();
    REQUIRE(p == 4);
}

TEST_CASE("Mutex_lock", "[stl_threads]") {
    _Shadow_STL_mutex_lock lock;
    int count = 0;
    std::thread t1([&lock, &count]() {
        // sleep 1 second
        std::this_thread::sleep_for(std::chrono::seconds(1));
        lock._M_acquire_lock();
        ++count;
        lock._M_release_lock();
    });
    lock._M_acquire_lock();
    REQUIRE(count == 0);
    lock._M_release_lock();
    t1.join();
    REQUIRE(count == 1);
}

TEST_CASE("Auto_lock", "[stl_threads]") {
    _Shadow_STL_mutex_lock lock;
    int count = 0;
    std::thread t1([&lock, &count]() {
        // sleep 1 second
        std::this_thread::sleep_for(std::chrono::seconds(1));
        _Shadow_STL_auto_lock auto_lock(lock);
        ++count;
        REQUIRE(count == 1);
    });
    {
        _Shadow_STL_auto_lock auto_lock(lock);
        REQUIRE(count == 0);
    }
    t1.join();
    REQUIRE(count == 1);
}


SHADOW_STL_END_NAMESPACE