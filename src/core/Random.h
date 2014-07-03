/**
 * @file    Random.h
 * @author  chenqiang01@7aurora.com
 * @date    Dec 23, 2013
 * @brief   基于std::mt19937的随机数生成器
 *
 *          随机数生成器:
 *              std::linear_congruential_engine    线性同余法
 *              std::mersenne_twister_engine       梅森旋转法
 *              std::substract_with_carry_engine   滞后Fibonacci
 */

#pragma once

#include <cstdint>
#include <random>
#include "Platform.h"


/**
 * A PRNG with one instance per thread. This PRNG uses a mersenne twister random
 * number generator.
 */
class Random
{
public:
    
    // initialize PRNG pointer and seed
    static void seed(int32_t seed = 0);

    /**
     * Returns a random uint32_t
     */
    static uint32_t rand32();

    /**
     * Returns a random uint32_t in [0, max). If max == 0, returns 0.
     */
    static uint32_t rand32(uint32_t max);

    /**
     * Returns a random uint32_t in [min, max). If min == max, returns 0.
     */
    static uint32_t rand32(uint32_t min, uint32_t max);

    /**
     * Returns a random uint64_t
     */
    static uint64_t rand64() 
    {
        return ((uint64_t)rand32() << 32) | rand32();
    }

    /**
     * Returns a random uint64_t in [0, max). If max == 0, returns 0.
     */
    static uint64_t rand64(uint64_t max);

    /**
     * Returns a random uint64_t in [min, max). If min == max, returns 0.
     */
    static uint64_t rand64(uint64_t min, uint64_t max);

    /**
     * Returns true 1/n of the time. If n == 0, always returns false
     */
    static bool oneIn(uint32_t n);

    /**
     * Returns a double in [0, 1)
     */
    static double randDouble01() 
    {
        return std::generate_canonical<double, std::numeric_limits<double>::digits>
            (*rng);
    }

    /**
     * Returns a double in [min, max), if min == max, returns 0.
     */
    static double randDouble(double min, double max);

private:
    static FOLLY_TLS std::default_random_engine*  rng;
};

/*
 * Return a good seed for a random number generator.
 * Note that this is a legacy function, as it returns a 32-bit value, which
 * is too small to be useful as a "real" RNG seed. Use the functions in class
 * Random instead.
 */
inline int32_t randomNumberSeed()
{
    Random::seed();
    return Random::rand32();
}
