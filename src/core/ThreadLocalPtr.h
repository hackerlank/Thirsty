/*
 *  @file   ThreadLocalPtr.h
 *  @author ichenq@gmail.com
 *  @date   Aug 8, 2014
 *  @brief  Implement thread local storage pointer
 *
 */
#pragma once

#include <cstdint>

namespace detail {

// allocates a thread local storage (TLS) index
uint32_t tls_create();

// releases a thread local storage (TLS) index
void tls_free(uint32_t id);

// stores a value in the calling thread's thread local storage (TLS) 
// slot for the specified TLS index
void tls_set(uint32_t id, void* ptr);

// retrieves the value in the calling thread's thread local storage (TLS) 
// slot for the specified TLS index
void* tls_get(uint32_t id);

} // namespace detail


template <typename T>
class ThreadLocalPtr
{
public:
    ThreadLocalPtr() : id_(detail::tls_create()) {}

    explicit ThreadLocalPtr(T* p)
        : id_(detail::tls_create())
    {
        detail::tls_set(id_, p);
    }

    ThreadLocalPtr(ThreadLocalPtr&& other)
        : id_(other.id_)
    {
        other.id_ = 0;
    }
    ~ThreadLocalPtr()
    {
        destroy();
    }

    // non-copyable
    ThreadLocalPtr(const ThreadLocalPtr&) = delete;
    ThreadLocalPtr& operator=(const ThreadLocalPtr&) = delete;

    T* get() const
    {
        return static_cast<T*>(detail::tls_get(id_));
    }

    T* operator->() const
    {
        return get();
    }

    T& operator*() const
    {
        return *get();
    }

    T* release()
    {
        T* ptr = get();
        if (ptr)
        {
            detail::tls_set(id_, nullptr);
        }
        return ptr;
    }

    void reset(T* ptr = nullptr)
    {
        T* old = release();
        delete old;
        if (ptr)
        {            
            detail::tls_set(id_, ptr);
        }
    }

    explicit operator bool() const
    {
        return get() != nullptr;
    }

private:
    void destroy()
    {
        if (id_)
        {
            T* ptr = release();
            delete ptr;
            detail::tls_free(id_);
            id_ = 0;
        }
    }

private:
    uint32_t   id_; // every instantiation has a unique id
};
