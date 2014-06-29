#include "Random.h"
#include <cassert>

Random::tls_rng_ptr  Random::rng;

void Random::seed(int32_t seed_value)
{
    if (seed_value == 0)
    {
        std::random_device device;
        seed_value = device();
    }
    if (!rng.get())
    {
        rng.reset(new std::default_random_engine);
    }
    rng->seed(seed_value);
}

uint32_t Random::rand32()
{
    assert(rng.get());
    uint32_t r = (*rng)();
    return r;
}

uint32_t Random::rand32(uint32_t max)
{
    assert(rng.get());
    if (max == 0)
    {
        return 0;
    }
    return std::uniform_int_distribution<uint32_t>(0, max - 1)(*rng);
}

uint32_t Random::rand32(uint32_t min, uint32_t max)
{
    assert(rng.get());
    if (min == max)
    {
        return 0;
    }
    return std::uniform_int_distribution<uint32_t>(min, max - 1)(*rng);
}

uint64_t Random::rand64(uint64_t max)
{
    assert(rng.get());
    if (max == 0)
    {
        return 0;
    }
    return std::uniform_int_distribution<uint64_t>(0, max - 1)(*rng);
}

uint64_t Random::rand64(uint64_t min, uint64_t max)
{
    assert(rng.get());
    if (min == max)
    {
        return 0;
    }
    return std::uniform_int_distribution<uint64_t>(min, max - 1)(*rng);
}

bool Random::oneIn(uint32_t n)
{
    if (n == 0)
    {
        return false;
    }
    return rand32(n) == 0;
}

double Random::randDouble(double min, double max)
{
    assert(rng.get());
    if (std::fabs(max - min) < std::numeric_limits<double>::epsilon())
    {
        return 0;
    }
    return std::uniform_real_distribution<double>(min, max)(*rng);
}
