#include "ThreadLocalPtr.h"
#include <cassert>

namespace detail {

#ifdef _WIN32

#include <windows.h>

uint32_t  tls_create()
{
    uint32_t id = TlsAlloc();
    assert(id != TLS_OUT_OF_INDEXES);
    return id;
}

void  tls_free(uint32_t id)
{
    int ok = TlsFree(id);
    assert(ok != 0);
}

void tls_set(uint32_t id, void* ptr)
{
    int ok = TlsSetValue(id, ptr);
    assert(ok != 0);
}

void* tls_get(uint32_t id)
{
    return TlsGetValue(id);
}

#elif defined(__linux__) || defined(__GNUC__)

#include <pthread.h>

uint32_t  tls_create()
{
    uint32_t id = 0;
    int ok = pthread_key_create((pthread_key_t*)&id, nullptr);
    assert(ok != 0);
    return id;
}

void  tls_free(uint32_t id)
{
    int ok = pthread_key_delete(id);
    assert(ok == 0);
}

void tls_set(uint32_t id, void* ptr)
{
    int ok = pthread_setspecific(id, ptr);
    assert(ok == 0);
}

void* tls_get(uint32_t id)
{
    return pthread_getspecific(id);
}

#else

#error("thread local storage unsupported in this platform!")

#endif

} // namespace detail
