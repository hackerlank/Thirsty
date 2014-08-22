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

// @author Andrei Alexandrescu (andrei.alexandrescu@fb.com)

#include "Benchmark.h"
#include <cmath>
#include <ctime>
#include <limits>
#include <vector>
#include <tuple>
#include <memory>
#include <regex>
#include <algorithm>
#include <iostream>
#include "core/Foreach.h"
#include "core/Conv.h"
#include "core/Strings.h"
#include "logging.h"


using namespace std;

BenchmarkSuspender::NanosecondsSpent BenchmarkSuspender::nsSpent;

namespace {

typedef function<detail::TimeIterPair(unsigned int)> BenchmarkFun;
typedef tuple<const char*, const char*, BenchmarkFun> BenchmarkItem;

// avoid static initialization order fiasco
vector<BenchmarkItem>& getenchmarks()
{
    static vector<BenchmarkItem> benchmarks;
    return benchmarks;
}

}


// Add the global baseline
BENCHMARK(globalBenchmarkBaseline, n)
{
#ifdef _MSC_VER
    _ReadWriteBarrier();
#else
    asm volatile("");
#endif
}

void detail::addBenchmarkImpl(const char* file,
                              const char* name,
                              BenchmarkFun fun)
{
    auto item = make_tuple(file, name, fun);
    getenchmarks().push_back(item);
}


/**
 * Given a bunch of benchmark samples, estimate the actual run time.
 */
static double estimateTime(double * begin, double * end)
{
    assert(begin < end);

    // Current state of the art: get the minimum. After some
    // experimentation, it seems taking the minimum is the best.

    return *min_element(begin, end);
}


static double runBenchmarkGetNSPerIteration(const BenchmarkFun& fun,
    const double globalBaseline, int32_t min_iters, int32_t min_usec,
    int32_t max_secs)
{
    static const auto minNanoseconds = min_usec * 1000UL;

    // We do measurements in several epochs and take the minimum, to
    // account for jitter.
    static const unsigned int epochs = 1000;

    // We establish a total time budget as we don't want a measurement
    // to take too long. This will curtail the number of actual epochs.
    const uint64_t timeBudgetInNs = max_secs * 1000000000;
    uint64_t global = getNowTickCount();

    double epochResults[epochs] = { 0 };
    size_t actualEpochs = 0;

    for (; actualEpochs < epochs; ++actualEpochs)
    {
        for (unsigned int n = min_iters; n < (1UL << 30); n *= 2)
        {
            auto const nsecsAndIter = fun(n); // run `n` times of benchmark case
            if (nsecsAndIter.first < minNanoseconds)
            {
                continue;
            }
            // We got an accurate enough timing, done. But only save if
            // smaller than the current result.
            epochResults[actualEpochs] = max(0.0, double(nsecsAndIter.first) /
                nsecsAndIter.second - globalBaseline);
            // Done with the current epoch, we got a meaningful timing.
            break;
        }
        if (getNowTickCount() - global >= timeBudgetInNs)
        {
            // No more time budget available.
            ++actualEpochs;
            break;
        }
    }
    // If the benchmark was basically drowned in baseline noise, it's
    // possible it became negative.
    return max(0.0, estimateTime(epochResults, epochResults + actualEpochs));
}

static void printBenchmarkResultsAsTable(
    const vector<tuple<const char*, const char*, double> >& data);

void runBenchmarks(const std::string& regexp, int32_t min_iters,
                   int32_t min_usec, int32_t max_secs)
{
    CHECK(!getenchmarks().empty());
    vector<tuple<const char*, const char*, double>> results;
    results.reserve(getenchmarks().size() - 1);

    unique_ptr<regex> bmRegex;
    if (!regexp.empty())
    {
        bmRegex.reset(new regex(regexp));
    }

    // PLEASE KEEP QUIET. MEASUREMENTS IN PROGRESS.
    auto benchmarks = getenchmarks();
    auto const globalBaseline = runBenchmarkGetNSPerIteration(
        get<2>(getenchmarks().front()), 0, min_iters, min_usec, max_secs);
    for (auto i = 1; i < benchmarks.size(); i++)
    {
        double elapsed = 0.0;
        if (strcmp(get<1>(benchmarks[i]), "-") != 0) // skip separators
        {
            if (bmRegex && regex_search(get<1>(benchmarks[i]), *bmRegex))
            {
                continue;
            }
            elapsed = runBenchmarkGetNSPerIteration(get<2>(benchmarks[i]),
                globalBaseline, min_iters, min_usec, max_secs);
        }
        results.emplace_back(get<0>(benchmarks[i]),
            get<1>(benchmarks[i]), elapsed);
    }

    // PLEASE MAKE NOISE. MEASUREMENTS DONE.

    printBenchmarkResultsAsTable(results);
}


struct ScaleInfo
{
    double boundary;
    const char* suffix;
};

static const ScaleInfo kTimeSuffixes[]
{
    { 365.25 * 24 * 3600, "years" },
    { 24 * 3600, "days" },
    { 3600, "hr" },
    { 60, "min" },
    { 1, "s" },
    { 1E-3, "ms" },
    { 1E-6, "us" },
    { 1E-9, "ns" },
    { 1E-12, "ps" },
    { 1E-15, "fs" },
    { 0, nullptr },
};

static const ScaleInfo kMetricSuffixes[]
{
    { 1E24, "Y" },  // yotta
    { 1E21, "Z" },  // zetta
    { 1E18, "X" },  // "exa" written with suffix 'X' so as to not create
    //   confusion with scientific notation
    { 1E15, "P" },  // peta
    { 1E12, "T" },  // terra
    { 1E9, "G" },   // giga
    { 1E6, "M" },   // mega
    { 1E3, "K" },   // kilo
    { 1, "" },
    { 1E-3, "m" },  // milli
    { 1E-6, "u" },  // micro
    { 1E-9, "n" },  // nano
    { 1E-12, "p" }, // pico
    { 1E-15, "f" }, // femto
    { 1E-18, "a" }, // atto
    { 1E-21, "z" }, // zepto
    { 1E-24, "y" }, // yocto
    { 0, nullptr },
};

static string humanReadable(double n, unsigned int decimals, const ScaleInfo* scales)

{
    if (std::isinf(n) || std::isnan(n))
    {
        return to<string>(n);
    }

    const double absValue = fabs(n);
    const ScaleInfo* scale = scales;
    while (absValue < scale[0].boundary && scale[1].suffix != nullptr)
    {
        ++scale;
    }

    const double scaledValue = n / scale->boundary;
    return stringPrintf("%.*f%s", decimals, scaledValue, scale->suffix);
}

static string readableTime(double n, unsigned int decimals)
{
    return humanReadable(n, decimals, kTimeSuffixes);
}

static string metricReadable(double n, unsigned int decimals)
{
    return humanReadable(n, decimals, kMetricSuffixes);
}

void printBenchmarkResultsAsTable(
    const vector<tuple<const char*, const char*, double> >& data)
{
    // Width available
    static const unsigned int columns = 76;

    // Compute the longest benchmark name
    size_t longestName = 0;
    auto benchmarks = getenchmarks();
    FOR_EACH_RANGE(i, 1, benchmarks.size())
    {
        longestName = max(longestName, strlen(get<1>(benchmarks[i])));
    }

    // Print a horizontal rule
    auto separator = [&](char pad)
    {
        puts(string(columns, pad).c_str());
    };

    // Print header for a file
    auto header = [&](const char* file)
    {
        separator('=');
        printf("%-*srelative  time/iter  iters/s\n",
            columns - 28, file);
        separator('=');
    };

    double baselineNsPerIter = numeric_limits<double>::max();
    const char* lastFile = "";

    for (auto& datum : data)
    {
        auto file = get<0>(datum);
        if (strcmp(file, lastFile))
        {
            // New file starting
            header(file);
            lastFile = file;
        }

        string s = get<1>(datum);
        if (s == "-")
        {
            separator('-');
            continue;
        }
        bool useBaseline /* = void */;
        if (s[0] == '%')
        {
            s.erase(0, 1);
            useBaseline = true;
        }
        else
        {
            baselineNsPerIter = get<2>(datum);
            useBaseline = false;
        }
        s.resize(columns - 29, ' ');
        auto nsPerIter = get<2>(datum);
        auto secPerIter = nsPerIter / 1E9;
        auto itersPerSec = 1 / secPerIter;
        if (!useBaseline)
        {
            // Print without baseline
            printf("%*s           %9s  %7s\n",
                static_cast<int>(s.size()), s.c_str(),
                readableTime(secPerIter, 2).c_str(),
                metricReadable(itersPerSec, 2).c_str());
        }
        else
        {
            // Print with baseline
            auto rel = baselineNsPerIter / nsPerIter * 100.0;
            printf("%*s %7.2f%%  %9s  %7s\n",
                static_cast<int>(s.size()), s.c_str(),
                rel,
                readableTime(secPerIter, 2).c_str(),
                metricReadable(itersPerSec, 2).c_str());
        }
    }
    separator('=');
}
