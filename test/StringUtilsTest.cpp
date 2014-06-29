#include "core/StringUtils.h"
#include <gtest/gtest.h>
#include <cinttypes>
#include <vector>

using namespace std;

TEST(StringUtils, StringPrintf)
{
    // basic
    EXPECT_EQ("abc", stringPrintf("%s", "abc"));
    EXPECT_EQ("abc", stringPrintf("%sbc", "a"));
    EXPECT_EQ("abc", stringPrintf("a%sc", "b"));
    EXPECT_EQ("abc", stringPrintf("ab%s", "c"));
    EXPECT_EQ("abc", stringPrintf("abc"));
    
    // numeric formats
    EXPECT_EQ("12", stringPrintf("%d", 12));
    EXPECT_EQ("5000000000", stringPrintf("%"PRIi64, 5000000000UL));
    EXPECT_EQ("5000000000", stringPrintf("%"PRIi64, 5000000000L));
    EXPECT_EQ("-5000000000", stringPrintf("%"PRIi64, -5000000000L));
    EXPECT_EQ("-1", stringPrintf("%d", 0xffffffff));
    EXPECT_EQ("-1", stringPrintf("%"PRIi64, 0xffffffffffffffff));
    EXPECT_EQ("-1", stringPrintf("%"PRIi64, 0xffffffffffffffffUL));

    EXPECT_EQ("7.7", stringPrintf("%1.1f", 7.7));
    EXPECT_EQ("7.7", stringPrintf("%1.1lf", 7.7));
    EXPECT_EQ("7.70000000000000020",
        stringPrintf("%.17f", 7.7));
    EXPECT_EQ("7.70000000000000020",
        stringPrintf("%.17lf", 7.7));
}

TEST(StringUtils, stringAppendf)
{
    string s;
    stringAppendf(&s, "a%s", "b");
    stringAppendf(&s, "%c", 'c');
    EXPECT_EQ(s, "abc");
    stringAppendf(&s, " %d", 123);
    EXPECT_EQ(s, "abc 123");
}

TEST(StringUtils, StringPrintfVariousSizes)
{
    // Test a wide variety of output sizes
    for (int i = 0; i < 100; ++i)
    {
        string expected(i + 1, 'a');
        EXPECT_EQ("X" + expected + "X", stringPrintf("X%sX", expected.c_str()));
    }

    EXPECT_EQ("abc12345678910111213141516171819202122232425xyz",
        stringPrintf("abc%d%d%d%d%d%d%d%d%d%d%d%d%d%d"
        "%d%d%d%d%d%d%d%d%d%d%dxyz",
        1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16,
        17, 18, 19, 20, 21, 22, 23, 24, 25));
}


TEST(StringUtils, oldStringPrintfTests)
{
    EXPECT_EQ(string("a/b/c/d"),
        stringPrintf("%s/%s/%s/%s", "a", "b", "c", "d"));

    EXPECT_EQ(string("    5    10"),
        stringPrintf("%5d %5d", 5, 10));

    // check printing w/ a big buffer
    for (int size = (1 << 8); size <= (1 << 15); size <<= 1) {
        string a(size, 'z');
        string b = stringPrintf("%s", a.c_str());
        EXPECT_EQ(a.size(), b.size());
    }
}

TEST(StringUtils, oldStringAppendf)
{
    string s = "hello";
    stringAppendf(&s, "%s/%s/%s/%s", "a", "b", "c", "d");
    EXPECT_EQ(string("helloa/b/c/d"), s);
}


template<template<class, class> class VectorType>
void splitTest() 
{
    VectorType<string, std::allocator<string> > parts;

    split(',', "a,b,c", parts);
    EXPECT_EQ(parts.size(), 3);
    EXPECT_EQ(parts[0], "a");
    EXPECT_EQ(parts[1], "b");
    EXPECT_EQ(parts[2], "c");
    parts.clear();

    split(',', StringPiece("a,b,c"), parts);
    EXPECT_EQ(parts.size(), 3);
    EXPECT_EQ(parts[0], "a");
    EXPECT_EQ(parts[1], "b");
    EXPECT_EQ(parts[2], "c");
    parts.clear();

    split(',', string("a,b,c"), parts);
    EXPECT_EQ(parts.size(), 3);
    EXPECT_EQ(parts[0], "a");
    EXPECT_EQ(parts[1], "b");
    EXPECT_EQ(parts[2], "c");
    parts.clear();

    split(',', "a,,c", parts);
    EXPECT_EQ(parts.size(), 3);
    EXPECT_EQ(parts[0], "a");
    EXPECT_EQ(parts[1], "");
    EXPECT_EQ(parts[2], "c");
    parts.clear();

    split(',', string("a,,c"), parts);
    EXPECT_EQ(parts.size(), 3);
    EXPECT_EQ(parts[0], "a");
    EXPECT_EQ(parts[1], "");
    EXPECT_EQ(parts[2], "c");
    parts.clear();

    split(',', "a,,c", parts, true);
    EXPECT_EQ(parts.size(), 2);
    EXPECT_EQ(parts[0], "a");
    EXPECT_EQ(parts[1], "c");
    parts.clear();

    split(',', string("a,,c"), parts, true);
    EXPECT_EQ(parts.size(), 2);
    EXPECT_EQ(parts[0], "a");
    EXPECT_EQ(parts[1], "c");
    parts.clear();

    split(',', string(",,a,,c,,,"), parts, true);
    EXPECT_EQ(parts.size(), 2);
    EXPECT_EQ(parts[0], "a");
    EXPECT_EQ(parts[1], "c");
    parts.clear();

    // test multiple split w/o clear
    split(',', ",,a,,c,,,", parts, true);
    EXPECT_EQ(parts.size(), 2);
    EXPECT_EQ(parts[0], "a");
    EXPECT_EQ(parts[1], "c");
    split(',', ",,a,,c,,,", parts, true);
    EXPECT_EQ(parts.size(), 4);
    EXPECT_EQ(parts[2], "a");
    EXPECT_EQ(parts[3], "c");
    parts.clear();

    // test splits that with multi-line delimiter
    split("ab", "dabcabkdbkab", parts, true);
    EXPECT_EQ(parts.size(), 3);
    EXPECT_EQ(parts[0], "d");
    EXPECT_EQ(parts[1], "c");
    EXPECT_EQ(parts[2], "kdbk");
    parts.clear();

    string orig = "ab2342asdfv~~!";
    split("", orig, parts, true);
    EXPECT_EQ(parts.size(), 1);
    EXPECT_EQ(parts[0], orig);
    parts.clear();

    split("452x;o38asfsajsdlfdf.j", "asfds", parts, true);
    EXPECT_EQ(parts.size(), 1);
    EXPECT_EQ(parts[0], "asfds");
    parts.clear();

    split("a", "", parts, true);
    EXPECT_EQ(parts.size(), 0);
    parts.clear();

    split("a", "", parts);
    EXPECT_EQ(parts.size(), 1);
    EXPECT_EQ(parts[0], "");
    parts.clear();

    split("a", StringPiece(), parts, true);
    EXPECT_EQ(parts.size(), 0);
    parts.clear();

    split("a", StringPiece(), parts);
    EXPECT_EQ(parts.size(), 1);
    EXPECT_EQ(parts[0], "");
    parts.clear();

    split("a", "abcdefg", parts, true);
    EXPECT_EQ(parts.size(), 1);
    EXPECT_EQ(parts[0], "bcdefg");
    parts.clear();

    orig = "All, , your base, are , , belong to us";
    split(", ", orig, parts, true);
    EXPECT_EQ(parts.size(), 4);
    EXPECT_EQ(parts[0], "All");
    EXPECT_EQ(parts[1], "your base");
    EXPECT_EQ(parts[2], "are ");
    EXPECT_EQ(parts[3], "belong to us");
    parts.clear();
    split(", ", orig, parts);
    EXPECT_EQ(parts.size(), 6);
    EXPECT_EQ(parts[0], "All");
    EXPECT_EQ(parts[1], "");
    EXPECT_EQ(parts[2], "your base");
    EXPECT_EQ(parts[3], "are ");
    EXPECT_EQ(parts[4], "");
    EXPECT_EQ(parts[5], "belong to us");
    parts.clear();

    orig = ", Facebook, rul,es!, ";
    split(", ", orig, parts, true);
    EXPECT_EQ(parts.size(), 2);
    EXPECT_EQ(parts[0], "Facebook");
    EXPECT_EQ(parts[1], "rul,es!");
    parts.clear();
    split(", ", orig, parts);
    EXPECT_EQ(parts.size(), 4);
    EXPECT_EQ(parts[0], "");
    EXPECT_EQ(parts[1], "Facebook");
    EXPECT_EQ(parts[2], "rul,es!");
    EXPECT_EQ(parts[3], "");
}

template<template<class, class> class VectorType>
void piecesTest() 
{
    VectorType<StringPiece, std::allocator<StringPiece> > pieces;
    VectorType<StringPiece, std::allocator<StringPiece> > pieces2;

    split(',', "a,b,c", pieces);
    EXPECT_EQ(pieces.size(), 3);
    EXPECT_EQ(pieces[0], "a");
    EXPECT_EQ(pieces[1], "b");
    EXPECT_EQ(pieces[2], "c");

    pieces.clear();

    split(',', "a,,c", pieces);
    EXPECT_EQ(pieces.size(), 3);
    EXPECT_EQ(pieces[0], "a");
    EXPECT_EQ(pieces[1], "");
    EXPECT_EQ(pieces[2], "c");
    pieces.clear();

    split(',', "a,,c", pieces, true);
    EXPECT_EQ(pieces.size(), 2);
    EXPECT_EQ(pieces[0], "a");
    EXPECT_EQ(pieces[1], "c");
    pieces.clear();

    split(',', ",,a,,c,,,", pieces, true);
    EXPECT_EQ(pieces.size(), 2);
    EXPECT_EQ(pieces[0], "a");
    EXPECT_EQ(pieces[1], "c");
    pieces.clear();

    // test multiple split w/o clear
    split(',', ",,a,,c,,,", pieces, true);
    EXPECT_EQ(pieces.size(), 2);
    EXPECT_EQ(pieces[0], "a");
    EXPECT_EQ(pieces[1], "c");
    split(',', ",,a,,c,,,", pieces, true);
    EXPECT_EQ(pieces.size(), 4);
    EXPECT_EQ(pieces[2], "a");
    EXPECT_EQ(pieces[3], "c");
    pieces.clear();

    // test multiple split rounds
    split(",", "a_b,c_d", pieces);
    EXPECT_EQ(pieces.size(), 2);
    EXPECT_EQ(pieces[0], "a_b");
    EXPECT_EQ(pieces[1], "c_d");
    split("_", pieces[0], pieces2);
    EXPECT_EQ(pieces2.size(), 2);
    EXPECT_EQ(pieces2[0], "a");
    EXPECT_EQ(pieces2[1], "b");
    pieces2.clear();
    split("_", pieces[1], pieces2);
    EXPECT_EQ(pieces2.size(), 2);
    EXPECT_EQ(pieces2[0], "c");
    EXPECT_EQ(pieces2[1], "d");
    pieces.clear();
    pieces2.clear();

    // test splits that with multi-line delimiter
    split("ab", "dabcabkdbkab", pieces, true);
    EXPECT_EQ(pieces.size(), 3);
    EXPECT_EQ(pieces[0], "d");
    EXPECT_EQ(pieces[1], "c");
    EXPECT_EQ(pieces[2], "kdbk");
    pieces.clear();

    string orig = "ab2342asdfv~~!";
    split("", orig.c_str(), pieces, true);
    EXPECT_EQ(pieces.size(), 1);
    EXPECT_EQ(pieces[0], orig);
    pieces.clear();

    split("452x;o38asfsajsdlfdf.j", "asfds", pieces, true);
    EXPECT_EQ(pieces.size(), 1);
    EXPECT_EQ(pieces[0], "asfds");
    pieces.clear();

    split("a", "", pieces, true);
    EXPECT_EQ(pieces.size(), 0);
    pieces.clear();

    split("a", "", pieces);
    EXPECT_EQ(pieces.size(), 1);
    EXPECT_EQ(pieces[0], "");
    pieces.clear();

    split("a", "abcdefg", pieces, true);
    EXPECT_EQ(pieces.size(), 1);
    EXPECT_EQ(pieces[0], "bcdefg");
    pieces.clear();

    orig = "All, , your base, are , , belong to us";
    split(", ", orig, pieces, true);
    EXPECT_EQ(pieces.size(), 4);
    EXPECT_EQ(pieces[0], "All");
    EXPECT_EQ(pieces[1], "your base");
    EXPECT_EQ(pieces[2], "are ");
    EXPECT_EQ(pieces[3], "belong to us");
    pieces.clear();
    split(", ", orig, pieces);
    EXPECT_EQ(pieces.size(), 6);
    EXPECT_EQ(pieces[0], "All");
    EXPECT_EQ(pieces[1], "");
    EXPECT_EQ(pieces[2], "your base");
    EXPECT_EQ(pieces[3], "are ");
    EXPECT_EQ(pieces[4], "");
    EXPECT_EQ(pieces[5], "belong to us");
    pieces.clear();

    orig = ", Facebook, rul,es!, ";
    split(", ", orig, pieces, true);
    EXPECT_EQ(pieces.size(), 2);
    EXPECT_EQ(pieces[0], "Facebook");
    EXPECT_EQ(pieces[1], "rul,es!");
    pieces.clear();
    split(", ", orig, pieces);
    EXPECT_EQ(pieces.size(), 4);
    EXPECT_EQ(pieces[0], "");
    EXPECT_EQ(pieces[1], "Facebook");
    EXPECT_EQ(pieces[2], "rul,es!");
    EXPECT_EQ(pieces[3], "");
    pieces.clear();

    const char* str = "a,b";
    split(',', StringPiece(str), pieces);
    EXPECT_EQ(pieces.size(), 2);
    EXPECT_EQ(pieces[0], "a");
    EXPECT_EQ(pieces[1], "b");
    EXPECT_EQ(pieces[0].start(), str);
    EXPECT_EQ(pieces[1].start(), str + 2);

    std::set<StringPiece> unique;
    splitTo<StringPiece>(":", "asd:bsd:asd:asd:bsd:csd::asd",
        std::inserter(unique, unique.begin()), true);
    EXPECT_EQ(unique.size(), 3);
    if (unique.size() == 3) 
    {
        EXPECT_EQ(*unique.begin(), "asd");
        EXPECT_EQ(*--unique.end(), "csd");
    }
}

TEST(StringUtils, splitVector) 
{
    splitTest<std::vector>();
    piecesTest<std::vector>();
}


TEST(StringUtils, splitFixed) 
{
    StringPiece a, b, c, d;

    EXPECT_TRUE(split<false>('.', "a.b.c.d", a, b, c, d));
    EXPECT_TRUE(split<false>('.', "a.b.c", a, b, c));
    EXPECT_TRUE(split<false>('.', "a.b", a, b));
    EXPECT_TRUE(split<false>('.', "a", a));

    EXPECT_TRUE(split('.', "a.b.c.d", a, b, c, d));
    EXPECT_TRUE(split('.', "a.b.c", a, b, c));
    EXPECT_TRUE(split('.', "a.b", a, b));
    EXPECT_TRUE(split('.', "a", a));

    EXPECT_TRUE(split<false>('.', "a.b.c", a, b, c));
    EXPECT_EQ("a", a);
    EXPECT_EQ("b", b);
    EXPECT_EQ("c", c);
    EXPECT_FALSE(split<false>('.', "a.b", a, b, c));
    EXPECT_TRUE(split<false>('.', "a.b.c", a, b));
    EXPECT_EQ("a", a);
    EXPECT_EQ("b.c", b);

    EXPECT_TRUE(split('.', "a.b.c", a, b, c));
    EXPECT_EQ("a", a);
    EXPECT_EQ("b", b);
    EXPECT_EQ("c", c);
    EXPECT_FALSE(split('.', "a.b.c", a, b));
    EXPECT_FALSE(split('.', "a.b", a, b, c));

    EXPECT_TRUE(split<false>('.', "a.b", a, b));
    EXPECT_EQ("a", a);
    EXPECT_EQ("b", b);
    EXPECT_FALSE(split<false>('.', "a", a, b));
    EXPECT_TRUE(split<false>('.', "a.b", a));
    EXPECT_EQ("a.b", a);

    EXPECT_TRUE(split('.', "a.b", a, b));
    EXPECT_EQ("a", a);
    EXPECT_EQ("b", b);
    EXPECT_FALSE(split('.', "a", a, b));
    EXPECT_FALSE(split('.', "a.b", a));
}


TEST(StringUtils, SplitFixedConvert)
{
    StringPiece a, d;
    int b;
    double c;

    EXPECT_TRUE(split(':', "a:13:14.7:b", a, b, c, d));
    EXPECT_EQ("a", a);
    EXPECT_EQ(13, b);
    EXPECT_NEAR(14.7, c, 1e-10);
    EXPECT_EQ("b", d);

    EXPECT_TRUE(split<false>(':', "b:14:15.3:c", a, b, c, d));
    EXPECT_EQ("b", a);
    EXPECT_EQ(14, b);
    EXPECT_NEAR(15.3, c, 1e-10);
    EXPECT_EQ("c", d);

    EXPECT_FALSE(split(':', "a:13:14.7:b", a, b, d));

    EXPECT_TRUE(split<false>(':', "a:13:14.7:b", a, b, d));
    EXPECT_EQ("a", a);
    EXPECT_EQ(13, b);
    EXPECT_EQ("14.7:b", d);

    EXPECT_ANY_THROW(split<false>(':', "a:13:14.7:b", a, b, c));
}


TEST(StringUtils, join) 
{
    string output;

    std::vector<int> empty = {};
    join(":", empty, output);
    EXPECT_TRUE(output.empty());

    vector<string> input1 = { "1", "23", "456", "" };
    join(':', input1, output);
    EXPECT_EQ(output, "1:23:456:");
    output = join(':', input1);
    EXPECT_EQ(output, "1:23:456:");

    auto input2 = { 1, 23, 456 };
    join("-*-", input2, output);
    EXPECT_EQ(output, "1-*-23-*-456");
    output = join("-*-", input2);
    EXPECT_EQ(output, "1-*-23-*-456");

    auto input3 = { 'f', 'a', 'c', 'e', 'b', 'o', 'o', 'k' };
    join("", input3, output);
    EXPECT_EQ(output, "facebook");

    vector<string> input4 = { "", "f", "a", "c", "e", "b", "o", "o", "k", "" };
    join("_", input4, output);
    EXPECT_EQ(output, "_f_a_c_e_b_o_o_k_");
}
