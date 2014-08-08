#include "Random.h"
#include "ThreadLocalPtr.h"
#include <cassert>
#include <random>

using namespace std;

// using thread local storage for thread-safety
static default_random_engine* get_tls_rng()
{
    static ThreadLocalPtr<default_random_engine>  rng;
    if (rng.get() == nullptr)
    {
        rng.reset(new default_random_engine());
    }
    assert(rng.get());
    return rng.get();
}

void Random::seed(int32_t seed_value)
{
    if (seed_value == 0)
    {
        random_device device;
        seed_value = device();
    }
    get_tls_rng()->seed(seed_value);
}

uint32_t Random::rand32()
{
    uint32_t r = (*get_tls_rng())();
    return r;
}

uint32_t Random::rand32(uint32_t max)
{
    if (max == 0)
    {
        return 0;
    }
    return std::uniform_int_distribution<uint32_t>(0, max - 1)(*get_tls_rng());
}

uint32_t Random::rand32(uint32_t min, uint32_t max)
{
    if (min == max)
    {
        return 0;
    }
    return std::uniform_int_distribution<uint32_t>(min, max - 1)(*get_tls_rng());
}

uint64_t Random::rand64(uint64_t max)
{
    if (max == 0)
    {
        return 0;
    }
    return std::uniform_int_distribution<uint64_t>(0, max - 1)(*get_tls_rng());
}

uint64_t Random::rand64(uint64_t min, uint64_t max)
{
    if (min == max)
    {
        return 0;
    }
    return std::uniform_int_distribution<uint64_t>(min, max - 1)(*get_tls_rng());
}

bool Random::oneIn(uint32_t n)
{
    if (n == 0)
    {
        return false;
    }
    return rand32(n) == 0;
}

double Random::randDouble01()
{
    return std::generate_canonical<double, std::numeric_limits<double>::digits>
        (*get_tls_rng());
}

double Random::randDouble(double min, double max)
{
    if (std::fabs(max - min) < std::numeric_limits<double>::epsilon())
    {
        return 0;
    }
    return std::uniform_real_distribution<double>(min, max)(*get_tls_rng());
}
