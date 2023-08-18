#ifndef SHADOW_STL_MEMORY
#define SHADOW_STL_MEMORY


#include "allocator/stl_alloc.h"
#include "include/stl_config.h"
#include "algorithm/stl_algobase.h"
#include "allocator/stl_construct.h"
#include "allocator/stl_unitialized.h"

SHADOW_STL_BEGIN_NAMESPACE

template <typename T>
struct auto_ptr_ref {
    T* _M_ptr;
    auto_ptr_ref(T* ptr) : _M_ptr(ptr) {}
};

// original smart pointer
template <typename T>
class auto_ptr {
private:
    T* _M_ptr;
public:
    using element_type = T;

    explicit auto_ptr(T* p = nullptr) noexcept : _M_ptr(p) {}

    auto_ptr(auto_ptr& a) noexcept : _M_ptr(a.release()) {}

    template <typename U>
    auto_ptr(auto_ptr<U>& a) noexcept : _M_ptr(a.release()) {}

    auto_ptr& operator=(auto_ptr& a) noexcept {
        if (&a != this) {
            delete _M_ptr;
            _M_ptr = a.release();
        }
        return *this;
    }

    template <typename T1>
    auto_ptr& operator=(auto_ptr<T1>& a) noexcept {
        if (&a != this) {
            delete _M_ptr;
            _M_ptr = a.release();
        }
        return *this;
    }

    // Note: The C++ standard says there is supposed to be an empty throw
    // specification here, but omitting it is standard conforming.  Its 
    // presence can be detected only if _Tp::~_Tp() throws, but (17.4.3.6/2)
    // this is prohibited.
    ~auto_ptr() {
        delete _M_ptr;
    }

    T& operator*() const noexcept {
        return *_M_ptr;
    }
    T* operator->() const noexcept {
        return _M_ptr;
    }

    T* get() const noexcept {
        return _M_ptr;
    }
    T* release() noexcept {
        T* tmp = _M_ptr;
        _M_ptr = nullptr;
        return tmp;
    }

    void reset(T* p = nullptr) noexcept {
        if (p != _M_ptr) {
            delete _M_ptr;
            _M_ptr = p;
        }
    }

    // According to the C++ standard, these conversions are required.  Most
    // present-day compilers, however, do not enforce that requirement---and, 
    // in fact, most present-day compilers do not support the language 
    // features that these conversions rely on.
    auto_ptr(auto_ptr_ref<T> ref) noexcept : _M_ptr(ref._M_ptr) {}
    auto_ptr& operator=(auto_ptr_ref<T> ref) noexcept {
        if (ref._M_ptr != this->get()) {
            delete _M_ptr;
            _M_ptr = ref._M_ptr;
        }
        return *this;
    }

    // Conversions to/from auto_ptr_ref<T> are not part of the C++ standard.
    template <typename T1> operator auto_ptr_ref<T1>() noexcept {
        return auto_ptr_ref<T1>(this->release());
    }

    template <typename T1> operator auto_ptr<T1>() noexcept {
        return auto_ptr<T1>(this->release());
    }

};

SHADOW_STL_END_NAMESPACE

#endif // SHADOW_STL_MEMORY