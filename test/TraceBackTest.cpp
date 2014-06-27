#include "core/traceback.h"
#include <gtest/gtest.h>
#include <iostream>


using namespace std;


void func1(const char* msg)
{
    throw traceback::OutOfRange(msg);
}

void func2(const char* msg)
{
    func1(msg);
}

void func3(const char* msg)
{
    func2(msg);
}

TEST(traceback, Simple) 
{
    try
    {
        func3("Simple traceback");
    }
    catch (const traceback::ErrorInfo& err)
    {
        cout << err.diagnostic() << endl;
    }
}

