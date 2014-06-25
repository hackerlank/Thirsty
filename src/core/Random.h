/**
* @file    Random.h
* @author  chenqiang01@7aurora.com
* @date    Dec 23, 2013
* @brief   ����std::mt19937�������������
*          ժ��: https://github.com/facebook/rocksdb/blob/master/util/random.h
*
*          �����������:
*              std::linear_congruential_engine    ����ͬ�෨
*              std::mersenne_twister_engine       ÷ɭ��ת��
*              std::substract_with_carry_engine   �ͺ�Fibonacci
*
*          �ֲ�����
*              std::uniform_int_distribution       �������ȷֲ�
*              std::uniform_real_distribution      ���������ȷֲ�
*/

#pragma once

#include <cstdint>
#include <random>


// A simple 64bit random number generator based on std::mt19937_64
class Random64 
{
public:
    explicit Random64(uint64_t s = 0) : generator_(s) { }

    // Generates the next random number
    uint64_t Next() { return generator_(); }

    // Returns a uniformly distributed value in the range [0..n-1]
    // REQUIRES: n > 0
    uint64_t Uniform(uint64_t n) {
        return std::uniform_int_distribution<uint64_t>(0, n - 1)(generator_);
    }

    // Randomly returns true ~"1/n" of the time, and false otherwise.
    // REQUIRES: n > 0
    bool OneIn(uint64_t n) { return Uniform(n) == 0; }

    // Skewed: pick "base" uniformly from range [0,max_log] and then
    // return "base" random bits.  The effect is to pick a number in the
    // range [0,2^max_log-1] with exponential bias towards smaller numbers.
    uint64_t Skewed(int max_log) {
        return Uniform(1 << Uniform(max_log + 1));
    }

private:
    std::mt19937_64     generator_;
};
