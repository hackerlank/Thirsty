#include <iostream>
#include <gtest/gtest.h>
#include "Benchmark.h"


using namespace std;

int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
    int r = RUN_ALL_TESTS();

    // run benchmarks
#ifdef NDEBUG
    cout << "\nPATIENCE, BENCHMARKS IN PROGRESS." << endl;
    runBenchmarks();
    cout << "MEASUREMENTS DONE." << endl;
#endif
    return r;
}
