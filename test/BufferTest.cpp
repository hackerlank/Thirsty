#include "net/Buffer.h"
#include <gtest/gtest.h>
#include <string>

using namespace std;

TEST(Buffer, BufferCtor)
{
    Buffer b1;
    
    string s = "hello, kitty";
    Buffer b2(s.data(), s.length());
    
}
