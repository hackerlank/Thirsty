#include <stdlib.h>
#include <time.h>
#include <objbase.h>
#include <gtest/gtest.h>
#include "net/TCPServer.h"


#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "winmm.lib")


class MyTestEnvironment : public testing::Environment
{
public:
    void  SetUp()
    {
        ::CoInitialize(NULL);
        srand((unsigned)time(NULL));
    }

    void  TearDown()
    {
        ::CoUninitialize();
    }
};


int main(int argc, char* argv[])
{
    testing::AddGlobalTestEnvironment(new MyTestEnvironment);
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
