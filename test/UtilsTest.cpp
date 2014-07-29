#include "Utils.h"
#include <gtest/gtest.h>
#include <vector>
#include <map>

TEST(UtilsTest, getNowTickCount)
{
    auto tick = getNowTickCount();
    EXPECT_GT(tick, 0);
    auto tick2 = getNowTickCount();
    EXPECT_GE(tick2, tick);
}

TEST(UtilsTest, bin2hex)
{
    StringPiece b("hello");
    EXPECT_EQ("68656c6c6f", bin2hex(b));
}


TEST(UtilsTest, MergeNonOverlapping) 
{
  std::vector<int> a = {0, 2, 4, 6};
  std::vector<int> b = {1, 3, 5, 7};
  std::vector<int> c;

  folly::merge(a.begin(), a.end(),
               b.begin(), b.end(),
               std::back_inserter(c));
  EXPECT_EQ(8, c.size());
  for (int i = 0; i < 8; ++i) 
  {
    EXPECT_EQ(i, c[i]);
  }
}

TEST(UtilsTest, MergeOverlappingInSingleInputRange)
{
  std::vector<std::pair<int, int>> a = {{0, 0}, {0, 1}};
  std::vector<std::pair<int, int>> b = {{2, 2}, {3, 3}};
  std::map<int, int> c;

  folly::merge(a.begin(), a.end(),
               b.begin(), b.end(),
               std::inserter(c, c.begin()));
  EXPECT_EQ(3, c.size());

  // First value is inserted, second is not
  EXPECT_EQ(c[0], 0);

  EXPECT_EQ(c[2], 2);
  EXPECT_EQ(c[3], 3);
}

TEST(UtilsTest, MergeOverlappingInDifferentInputRange)
{
  std::vector<std::pair<int, int>> a = {{0, 0}, {1, 1}};
  std::vector<std::pair<int, int>> b = {{0, 2}, {3, 3}};
  std::map<int, int> c;

  folly::merge(a.begin(), a.end(),
               b.begin(), b.end(),
               std::inserter(c, c.begin()));
  EXPECT_EQ(3, c.size());

  // Value from a is inserted, value from b is not.
  EXPECT_EQ(c[0], 0);

  EXPECT_EQ(c[1], 1);
  EXPECT_EQ(c[3], 3);
}
