#include <stdlib.h>
#include <time.h>
#include <gtest/gtest.h>


#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "winmm.lib")


class MyTestEnvironment : public testing::Environment
{
public:
    void  SetUp()
    {
        srand((unsigned)time(NULL));
    }

    void  TearDown()
    {
    }
};


int main(int argc, char* argv[])
{
    testing::AddGlobalTestEnvironment(new MyTestEnvironment);
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
