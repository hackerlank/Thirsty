#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <gtest/gtest.h>
#include "core/logging.h"
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
        runBenchmarks();
        return r;
    }
    catch (traceback::ErrorInfo& err)
    {
        cout << err.diagnostic() << endl;
        return 1;
    }
    catch(exception& ex)
    {
        cout << ex.what() << endl;
    }
}
