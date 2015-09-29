#include "dynamic.h"
#include <limits.h>
#include <iostream>
#include <gtest/gtest.h>

using std::string;

TEST(dynamic, NullBool)
{
    dynamic d1(nullptr);
    EXPECT_TRUE(d1.isNull());
    EXPECT_EQ(d1.as<string>(), "null");
    EXPECT_EQ(d1.dump(), "null");
    EXPECT_THROW(d1.getInt(), dynamic::TypeError);

    bool f1 = false;
    dynamic d2(f1);
    EXPECT_TRUE(d2.isBool());
    EXPECT_FALSE(d2.getBool());
    EXPECT_FALSE(d2.as<bool>());
    EXPECT_EQ(d2.as<string>(), "false");
    EXPECT_EQ(d2.dump(), "false");
    EXPECT_THROW(d2.getInt(), dynamic::TypeError);

    bool f2 = d2;
    EXPECT_EQ(f2, f1);
}

template < typename Number >
void test_dynamic_number(Number n)
{
    dynamic d(n);
    EXPECT_TRUE(d.isNumber());
    EXPECT_EQ(d.as<Number>(), n);
    EXPECT_EQ(d.as<string>(), d.dump());
    EXPECT_THROW(d.size(), dynamic::TypeError);
    
    Number m = d;
    EXPECT_EQ(m, n);
}

TEST(dynamic, NumberBasics)
{
    test_dynamic_number<int8_t>(-120);
    test_dynamic_number<uint8_t>(128);
    test_dynamic_number<int16_t>(-32550);
    test_dynamic_number<uint16_t>(65535);
    test_dynamic_number<int32_t>(INT_MIN);
    test_dynamic_number<uint32_t>(INT_MAX);
    test_dynamic_number<int64_t>(LLONG_MIN);
    test_dynamic_number<uint64_t>(LLONG_MAX);
    test_dynamic_number<float>(-0.168f);
    test_dynamic_number<double>(3.14159);
}

TEST(dynamic, string)
{
    const char* text = "a brown quick fox jumps over the lazy dog";
    string s = text;
    dynamic d1(text, strlen(text));
    EXPECT_EQ(d1.size(), s.size());
    EXPECT_EQ(d1.getString(), s);
    EXPECT_EQ(d1.as<string>(), s);

    dynamic d2(std::move(s));
    EXPECT_TRUE(s.empty());
    EXPECT_EQ(d2.as<string>(), text);

    string s2 = d2;
    EXPECT_EQ(s2, text);
}

TEST(dynamic, array)
{
    dynamic array1 = {10, 20, 30};
    EXPECT_TRUE(array1.isArray());
    EXPECT_EQ(array1.size(), 3);
    array1.push_back(40);
    EXPECT_EQ(array1.size(), 4);
    for (auto i = 0U; i < array1.size(); i++)
    {
        int n = array1.at(i);
        EXPECT_EQ(n, (i + 1) * 10);
    }

    dynamic array2 = { nullptr, 1024, true, "bonjour", 3.14159, array1};
    EXPECT_EQ(array2.size(), 6);
    auto elem1 = array2.at(0);
    EXPECT_TRUE(elem1.isNull());
    int elem2 = array2.at(1);
    EXPECT_EQ(elem2, 1024);
    bool elem3 = array2.at(2);
    EXPECT_EQ(elem3, true);
    std::string elem4 = array2.at(3);
    EXPECT_EQ(elem4, "bonjour");
    double elem5 = array2.at(4);
    EXPECT_EQ(elem5, 3.14159);
    auto elem6 = array2.at(5);
    EXPECT_TRUE(elem6.isArray());
}

TEST(dynamic, object)
{
    dynamic obj = dynamic::object;
    EXPECT_TRUE(obj.isObject());
    EXPECT_TRUE(obj.empty());

    obj["key1"] = 1024;
    obj["key2"] = false;
    obj["key3"] = "bonjour";
    EXPECT_EQ(obj.size(), 3);

    EXPECT_TRUE(obj["key1"].isInt());
    EXPECT_TRUE(obj["key2"].isBool());
    EXPECT_TRUE(obj["key3"].isString());

    auto s = obj.dump();
    EXPECT_EQ(s, "{\"key1\":1024,\"key2\":false,\"key3\":bonjour}");

    for (const auto& item : obj)
    {
        const string& key = item.first;
        EXPECT_EQ(key.substr(2), "key");
    }
}