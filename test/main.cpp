#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <gtest/gtest.h>
#include "core/Benchmark.h"

#ifdef _WIN32
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "winmm.lib")
#endif

using namespace std;

int main(int argc, char* argv[])
{
    try
    {
        testing::InitGoogleTest(&argc, argv);
        srand((unsigned)time(NULL));
        auto r = RUN_ALL_TESTS();

        // run benchmarks in release mode
#ifdef NDEBUG
        cout << "patience, running benchmarks..." << endl;
        runBenchmarks();
#endif
        return r;
    }
    catch(const std::exception& ex)
    {
        cout << ex.what() << endl;
    }
}
