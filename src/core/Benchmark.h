/*
 * Copyright 2014 Facebook, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <cstdint>
#include <string>
#include <utility>
#include <functional>
#include "Preprocessor.h"

/**
 * Runs all benchmarks defined. Usually put in main().
 * `regexp`     Only benchmarks whose names match this regex will be run
 * `min_iters`  Minimum # of iterations we'll try for each benchmark
 * `min_usec`   Minimum # of microseconds we'll accept for each benchmark
 * `max_secs`   Maximum # of seconds we'll spend on each benchmark
 */
void runBenchmarks(const std::string& regexp = "",
                   int32_t min_iters = 1,
                   int32_t min_usec = 100,
                   int32_t max_secs = 1);


namespace detail {

typedef std::pair<uint64_t, unsigned int> TimeIterPair;

/**
 * Adds a benchmark wrapped in a std::function. Only used
 * internally. Pass by value is intentional.
 */
void addBenchmarkImpl(const char* file,
                      const char* name,
                      std::function<TimeIterPair(unsigned int)>);

} // namespace detail


// Get current tick count(in nanoseconds), for time measurements
uint64_t getNowTickCount();


/**
 * Supporting type for BENCHMARK_SUSPEND defined below.
 */
struct BenchmarkSuspender
{
    BenchmarkSuspender() : start_(getNowTickCount())
    {
    }

    BenchmarkSuspender(const BenchmarkSuspender &) = delete;
    BenchmarkSuspender(BenchmarkSuspender&& rhs)
        : start_(rhs.start_)
    {
        rhs.start_ = getNowTickCount();
    }

    BenchmarkSuspender& operator=(const BenchmarkSuspender &) = delete;
    BenchmarkSuspender& operator=(BenchmarkSuspender && rhs)
    {
        start_ = rhs.start_;
        rhs.start_ = getNowTickCount();
        return *this;
    }

    ~BenchmarkSuspender()
    {
        tally();
    }

    void dismiss()
    {
        tally();
        start_ = getNowTickCount();
    }

    void rehire()
    {
        start_ = getNowTickCount();
    }

    /**
     * This helps the macro definition. To get around the dangers of
     * operator bool, returns a pointer to member (which allows no
     * arithmetic).
     */
    operator int BenchmarkSuspender::*() const
    {
        return nullptr;
    }

    /**
     * Accumulates nanoseconds spent outside benchmark.
     */
    typedef uint64_t NanosecondsSpent;
    static NanosecondsSpent nsSpent;

private:
    void tally()
    {
        auto end = getNowTickCount();
        nsSpent += (end - start_);
        start_ = end;
    }

    uint64_t    start_;
};

/**
 * Adds a benchmark. Usually not called directly but instead through
 * the macro BENCHMARK defined below. The lambda function involved
 * must take exactly one parameter of type unsigned, and the benchmark
 * uses it with counter semantics (iteration occurs inside the
 * function).
 */
template <typename Lambda>
void addBenchmarkWithTimes(const char* file, const char* name, Lambda&& lambda)
{
    auto execute = [=](unsigned int times)
    {
        BenchmarkSuspender::nsSpent = 0;
        unsigned int niter;

        // CORE MEASUREMENT STARTS
        auto const start = getNowTickCount();
        niter = lambda(times);
        auto const end = getNowTickCount();
        // CORE MEASUREMENT ENDS

        return detail::TimeIterPair(end - start - BenchmarkSuspender::nsSpent, niter);
    };

    detail::addBenchmarkImpl(file, name,
        std::function<detail::TimeIterPair(unsigned int)>(execute));
}

template <typename Lambda>
void addBenchmark(const char* file, const char* name, Lambda&& lambda) 
{
    addBenchmarkWithTimes(file, name, [=](unsigned int times) 
    {
        unsigned int niter = 0;
        while (times-- > 0) {
            niter += lambda();
        }
        return niter;
    });
}

/**
 * Call doNotOptimizeAway(var) against variables that you use for
 * benchmarking but otherwise are useless. The compiler tends to do a
 * good job at eliminating unused variables, and this function fools
 * it into thinking var is in fact needed.
 */
#ifdef _MSC_VER

#pragma optimize("", off)

template <class T>
void doNotOptimizeAway(T&& datum)
{
    datum = datum;
}

#pragma optimize("", on)

#else
template <class T>
void doNotOptimizeAway(T&& datum)
{
    asm volatile("" : "+r" (datum));
}
#endif



/**
 * Introduces a benchmark function. Used internally, see BENCHMARK and
 * friends below.
 */
#define BENCHMARK_IMPL(funName, stringName, paramType, paramName)       \
  static void funName();                                                \
  static bool FB_ANONYMOUS_VARIABLE(follyBenchmarkUnused) = (           \
      addBenchmark(__FILE__, stringName,                                \
      [](paramType paramName) -> unsigned { funName();                  \
        return 1; }), true);                                            \
  static void funName(paramType paramName)

/**
 * Introduces a benchmark function with support for returning the actual
 * number of iterations. Used internally, see BENCHMARK_MULTI and friends
 * below.
 */
#define BENCHMARK_MULTI_IMPL(funName, stringName, paramType, paramName) \
  static unsigned funName(paramType);                                   \
  static bool FB_ANONYMOUS_VARIABLE(follyBenchmarkUnused) = (           \
      addBenchmark(__FILE__, stringName,                                \
      [](paramType paramName) { return funName(paramName); }),          \
    true);                                                              \
  static unsigned funName(paramType paramName)

/**
 * Introduces a benchmark function. Use with two arguments. The first 
 * is the name of the benchmark. Use something descriptive, such as 
 * insertVectorBegin. The second argument may be missing, or could be 
 * a symbolic counter. The counter dictates how many internal iteration 
 * the benchmark does. Example:
 *
 * BENCHMARK(vectorPushBack) 
 * {
 *   vector<int> v;
 *   v.push_back(42);
 * }
 */
#define BENCHMARK(name)         \
      BENCHMARK_IMPL(           \
        name,                   \
        FB_STRINGIZE(name),     \
        __VA_ARGS__,            \
        __VA_ARGS__)

/**
 * Draws a line of dashes.
 */
#define BENCHMARK_DRAW_LINE()                                       \
  static bool FB_ANONYMOUS_VARIABLE(follyBenchmarkUnused) = (       \
    addBenchmark(__FILE__, "-", []() -> unsigned { return 0; }),    \
    true);

/**
 * Allows execution of code that doesn't count torward the benchmark's
 * time budget. Example:
 *
 * BENCHMARK_START_GROUP(insertVectorBegin, n) {
 *   vector<int> v;
 *   BENCHMARK_SUSPEND {
 *     v.reserve(n);
 *   }
 *   FOR_EACH_RANGE (i, 0, n) {
 *     v.insert(v.begin(), 42);
 *   }
 * }
 */
#define BENCHMARK_SUSPEND                               \
  if (auto FB_ANONYMOUS_VARIABLE(BENCHMARK_SUSPEND) =   \
      BenchmarkSuspender()) {}                          \
  else
