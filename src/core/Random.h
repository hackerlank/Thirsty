/**
* @file    Random.h
* @author  chenqiang01@7aurora.com
* @date    Dec 23, 2013
* @brief   基于std::mt19937的随机数生成器
*          摘自: https://github.com/facebook/rocksdb/blob/master/util/random.h
*
*          随机数生成器:
*              std::linear_congruential_engine    线性同余法
*              std::mersenne_twister_engine       梅森旋转法
*              std::substract_with_carry_engine   滞后Fibonacci
*
*          分布类型
*              std::uniform_int_distribution       整数均匀分布
*              std::uniform_real_distribution      浮点数均匀分布
*/

#pragma once

#include <cstdint>
#include <random>

class Random
{
public:
    explicit Random()
        : generator_(std::random_device()())
    { }

    // Generates the next random number
    uint32_t Next()
    {
        return generator_();
    }

    // Randomly returns true ~"1/n" of the time, and false otherwise.
    // REQUIRES: n > 0
    bool OneIn(uint32_t n) { return Uniform(n) == 0; }

    // Returns a uniformly distributed value in the range [0..n-1]
    // REQUIRES: n > 0
    uint32_t Uniform(uint32_t n)
    {
        return std::uniform_int_distribution<uint32_t>(0, n - 1)(generator_);
    }

private:
    std::default_random_engine       generator_;
};
