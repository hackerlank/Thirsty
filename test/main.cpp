#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <gtest/gtest.h>
#include "Benchmark.h"
#include "logging.h"

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
        SetLogHandler(NULL);

        int r = RUN_ALL_TESTS();

        // run benchmarks
#ifdef NDEBUG
        cout << "\nPATIENCE, BENCHMARKS IN PROGRESS." << endl;
        runBenchmarks();
        cout << "MEASUREMENTS DONE." << endl;
#endif
        return r;
    }
    catch(const std::exception& ex)
    {
        cout << ex.what() << endl;
    }
}
