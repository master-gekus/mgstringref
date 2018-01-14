#include "mgstringref_test.h"

void StandardAllocator::CompareTest(const mg::stringref& s, const mg::wstringref& ws)
{
    using namespace mg;

    stringref empty;
    wstringref wempty;
    stringref less1("aaa");
    stringref less2("cc");
    stringref greater1("ddd");
    stringref greater2("cccc");
    wstringref wless1(L"aaa");
    wstringref wless2(L"cc");
    wstringref wgreater1(L"ddd");
    wstringref wgreater2(L"cccc");

    EXPECT_GT(s.compare(nullptr), 0);
    EXPECT_GT(ws.compare(nullptr), 0);
    EXPECT_GT(s.compare(""), 0);
    EXPECT_GT(ws.compare(L""), 0);
    EXPECT_EQ(s.compare("ccc"), 0);
    EXPECT_EQ(ws.compare(L"ccc"), 0);
    EXPECT_GT(s.compare("cc"), 0);
    EXPECT_GT(ws.compare(L"cc"), 0);
    EXPECT_LT(s.compare("cccc"), 0);
    EXPECT_LT(ws.compare(L"cccc"), 0);
    EXPECT_EQ(s.compare("ccccccc", 3), 0);
    EXPECT_EQ(ws.compare(L"ccccccc", 3), 0);

    EXPECT_GT(s.compare(std::string{}), 0);
    EXPECT_GT(ws.compare(std::wstring{}), 0);
    EXPECT_EQ(s.compare(std::string("ccc")), 0);
    EXPECT_EQ(ws.compare(std::wstring(L"ccc")), 0);
    EXPECT_GT(s.compare(std::string("cc")), 0);
    EXPECT_GT(ws.compare(std::wstring(L"cc")), 0);
    EXPECT_LT(s.compare(std::string("cccc")), 0);
    EXPECT_LT(ws.compare(std::wstring(L"cccc")), 0);
    EXPECT_EQ(s.compare(std::string("ccccccc", 3)), 0);
    EXPECT_EQ(ws.compare(std::wstring(L"ccccccc", 3)), 0);

    EXPECT_GT(s.compare(empty), 0);
    EXPECT_GT(ws.compare(wempty), 0);
    EXPECT_GT(s.compare(less1), 0);
    EXPECT_GT(ws.compare(wless1), 0);
    EXPECT_GT(s.compare(less2), 0);
    EXPECT_GT(ws.compare(wless2), 0);
    EXPECT_LT(s.compare(greater1), 0);
    EXPECT_LT(ws.compare(wgreater1), 0);
    EXPECT_LT(s.compare(greater2), 0);
    EXPECT_LT(ws.compare(wgreater2), 0);
}

void CustomAllocator::CompareTest(const inplace::stringref& s, const inplace::wstringref& ws)
{
    using namespace inplace;

    size_t save_count = a.used_block_count();
    stringref empty(a);
    wstringref wempty(a);
    stringref less1("aaa", a);
    stringref less2("cc", a);
    stringref greater1("ddd", a);
    stringref greater2("cccc", a);
    wstringref wless1(L"aaa", a);
    wstringref wless2(L"cc", a);
    wstringref wgreater1(L"ddd", a);
    wstringref wgreater2(L"cccc", a);
    EXPECT_EQ(a.used_block_count(), save_count);

    EXPECT_GT(s.compare(nullptr), 0);
    EXPECT_GT(ws.compare(nullptr), 0);
    EXPECT_GT(s.compare(""), 0);
    EXPECT_GT(ws.compare(L""), 0);
    EXPECT_EQ(s.compare("ccc"), 0);
    EXPECT_EQ(ws.compare(L"ccc"), 0);
    EXPECT_GT(s.compare("cc"), 0);
    EXPECT_GT(ws.compare(L"cc"), 0);
    EXPECT_LT(s.compare("cccc"), 0);
    EXPECT_LT(ws.compare(L"cccc"), 0);
    EXPECT_EQ(s.compare("ccccccc", 3), 0);
    EXPECT_EQ(ws.compare(L"ccccccc", 3), 0);

    EXPECT_GT(s.compare(std::string{}), 0);
    EXPECT_GT(ws.compare(std::wstring{}), 0);
    EXPECT_EQ(s.compare(std::string("ccc")), 0);
    EXPECT_EQ(ws.compare(std::wstring(L"ccc")), 0);
    EXPECT_GT(s.compare(std::string("cc")), 0);
    EXPECT_GT(ws.compare(std::wstring(L"cc")), 0);
    EXPECT_LT(s.compare(std::string("cccc")), 0);
    EXPECT_LT(ws.compare(std::wstring(L"cccc")), 0);
    EXPECT_EQ(s.compare(std::string("ccccccc", 3)), 0);
    EXPECT_EQ(ws.compare(std::wstring(L"ccccccc", 3)), 0);

    EXPECT_GT(s.compare(empty), 0);
    EXPECT_GT(ws.compare(wempty), 0);
    EXPECT_GT(s.compare(less1), 0);
    EXPECT_GT(ws.compare(wless1), 0);
    EXPECT_GT(s.compare(less2), 0);
    EXPECT_GT(ws.compare(wless2), 0);
    EXPECT_LT(s.compare(greater1), 0);
    EXPECT_LT(ws.compare(wgreater1), 0);
    EXPECT_LT(s.compare(greater2), 0);
    EXPECT_LT(ws.compare(wgreater2), 0);
    EXPECT_EQ(a.used_block_count(), save_count);
}

TEST_F(StandardAllocator, CompareEmpty)
{
    using namespace mg;
    stringref s;
    wstringref ws;
    stringref empty;
    wstringref wempty;
    stringref letter("a");
    wstringref wletter(L"a");

    EXPECT_EQ(s.compare(nullptr), 0);
    EXPECT_EQ(ws.compare(nullptr), 0);
    EXPECT_EQ(s.compare(""), 0);
    EXPECT_EQ(ws.compare(L""), 0);
    EXPECT_LT(s.compare("a"), 0);
    EXPECT_LT(ws.compare(L"a"), 0);
    EXPECT_EQ(s.compare(std::string{}), 0);
    EXPECT_EQ(ws.compare(std::wstring{}), 0);
    EXPECT_LT(s.compare(std::string("a")), 0);
    EXPECT_LT(ws.compare(std::wstring(L"a")), 0);
    EXPECT_LT(s.compare(std::string("\0", 1)), 0);
    EXPECT_LT(ws.compare(std::wstring(L"a", 2)), 0);
    EXPECT_EQ(s.compare(empty), 0);
    EXPECT_EQ(ws.compare(wempty), 0);
    EXPECT_LT(s.compare(letter), 0);
    EXPECT_LT(ws.compare(wletter), 0);
}

TEST_F(CustomAllocator, CompareEmpty)
{
    using namespace inplace;
    stringref s(a);
    wstringref ws(a);
    stringref empty(a);
    wstringref wempty(a);
    stringref letter("a", a);
    wstringref wletter(L"a", a);
    EXPECT_EQ(a.used_block_count(), static_cast<std::size_t>(0));

    EXPECT_EQ(s.compare(nullptr), 0);
    EXPECT_EQ(ws.compare(nullptr), 0);
    EXPECT_EQ(s.compare(""), 0);
    EXPECT_EQ(ws.compare(L""), 0);
    EXPECT_LT(s.compare("a"), 0);
    EXPECT_LT(ws.compare(L"a"), 0);
    EXPECT_EQ(s.compare(std::string{}), 0);
    EXPECT_EQ(ws.compare(std::wstring{}), 0);
    EXPECT_LT(s.compare(std::string("a")), 0);
    EXPECT_LT(ws.compare(std::wstring(L"a")), 0);
    EXPECT_LT(s.compare(std::string("\0", 1)), 0);
    EXPECT_LT(ws.compare(std::wstring(L"a", 2)), 0);
    EXPECT_EQ(s.compare(empty), 0);
    EXPECT_EQ(ws.compare(wempty), 0);
    EXPECT_LT(s.compare(letter), 0);
    EXPECT_LT(ws.compare(wletter), 0);
}

TEST_F(StandardAllocator, CompareFullRef)
{
    using namespace mg;
    stringref s("ccc");
    wstringref ws(L"ccc");

    CompareTest(s, ws);
}

TEST_F(CustomAllocator, CompareFullRef)
{
    using namespace inplace;
    stringref s("ccc", a);
    wstringref ws(L"ccc", a);
    EXPECT_EQ(a.used_block_count(), static_cast<std::size_t>(0));

    CompareTest(s, ws);
}

TEST_F(StandardAllocator, ComparePartialRef)
{
    using namespace mg;
    stringref s("aaacccddd", 3, 3);
    wstringref ws(L"aaacccddd", 3, 3);

    CompareTest(s, ws);
}

TEST_F(CustomAllocator, ComparePartialRef)
{
    using namespace inplace;
    stringref s("aaacccddd", 3, 3, a);
    wstringref ws(L"aaacccddd", 3, 3, a);
    EXPECT_EQ(a.used_block_count(), static_cast<std::size_t>(0));

    CompareTest(s, ws);
}

TEST_F(StandardAllocator, CompareFullCopy)
{
    using namespace mg;
    stringref s(std::string("ccc"));
    wstringref ws(std::wstring(L"ccc"));

    CompareTest(s, ws);
}

TEST_F(CustomAllocator, CompareFullCopy)
{
    using namespace inplace;
    stringref s(std::string("ccc"), a);
    wstringref ws(std::wstring(L"ccc"), a);
    EXPECT_EQ(a.used_block_count(), static_cast<std::size_t>(2));

    CompareTest(s, ws);
}

TEST_F(StandardAllocator, ComparePartialCopy)
{
    using namespace mg;
    stringref s(std::string("aaacccddd"), 3, 3);
    wstringref ws(std::wstring(L"aaacccddd"), 3, 3);

    CompareTest(s, ws);
}

TEST_F(CustomAllocator, ComparePartialCopy)
{
    using namespace inplace;
    stringref s(std::string("aaacccddd"), 3, 3, a);
    wstringref ws(std::wstring(L"aaacccddd"), 3, 3, a);
    EXPECT_EQ(a.used_block_count(), static_cast<std::size_t>(2));

    CompareTest(s, ws);
}

TEST_F(StandardAllocator, CompareOperators)
{
    using namespace mg;
    stringref s("ccc");
    wstringref ws(L"ccc");

    EXPECT_TRUE(s == "ccc");
    EXPECT_TRUE(s != "ddd");
    EXPECT_TRUE(s < "ddd");
    EXPECT_TRUE(s <= "ddd");
    EXPECT_TRUE(s <= "ccc");
    EXPECT_TRUE(s > "bbb");
    EXPECT_TRUE(s >= "bbb");
    EXPECT_TRUE(s >= "ccc");

    EXPECT_TRUE("ccc" == s);
    EXPECT_TRUE("ddd" != s);
    EXPECT_TRUE("bbb" < s);
    EXPECT_TRUE("bbb" <= s);
    EXPECT_TRUE("ccc" <= s);
    EXPECT_TRUE("ddd" > s);
    EXPECT_TRUE("ddd" >= s);
    EXPECT_TRUE("ccc" >= s);

    EXPECT_TRUE(ws == L"ccc");
    EXPECT_TRUE(ws != L"ddd");
    EXPECT_TRUE(ws < L"ddd");
    EXPECT_TRUE(ws <= L"ddd");
    EXPECT_TRUE(ws <= L"ccc");
    EXPECT_TRUE(ws > L"bbb");
    EXPECT_TRUE(ws >= L"bbb");
    EXPECT_TRUE(ws >= L"ccc");

    EXPECT_TRUE(L"ccc" == ws);
    EXPECT_TRUE(L"ddd" != ws);
    EXPECT_TRUE(L"bbb" < ws);
    EXPECT_TRUE(L"bbb" <= ws);
    EXPECT_TRUE(L"ccc" <= ws);
    EXPECT_TRUE(L"ddd" > ws);
    EXPECT_TRUE(L"ddd" >= ws);
    EXPECT_TRUE(L"ccc" >= ws);
}

TEST_F(CustomAllocator, CompareOperators)
{
    using namespace inplace;
    stringref s("ccc", a);
    wstringref ws(L"ccc", a);

    EXPECT_TRUE(s == "ccc");
    EXPECT_TRUE(s != "ddd");
    EXPECT_TRUE(s < "ddd");
    EXPECT_TRUE(s <= "ddd");
    EXPECT_TRUE(s <= "ccc");
    EXPECT_TRUE(s > "bbb");
    EXPECT_TRUE(s >= "bbb");
    EXPECT_TRUE(s >= "ccc");
    EXPECT_TRUE(s == mg::stringref("ccc"));

    EXPECT_TRUE("ccc" == s);
    EXPECT_TRUE("ddd" != s);
    EXPECT_TRUE("bbb" < s);
    EXPECT_TRUE("bbb" <= s);
    EXPECT_TRUE("ccc" <= s);
    EXPECT_TRUE("ddd" > s);
    EXPECT_TRUE("ddd" >= s);
    EXPECT_TRUE("ccc" >= s);
    EXPECT_TRUE(mg::stringref("ccc") == s);

    EXPECT_TRUE(ws == L"ccc");
    EXPECT_TRUE(ws != L"ddd");
    EXPECT_TRUE(ws < L"ddd");
    EXPECT_TRUE(ws <= L"ddd");
    EXPECT_TRUE(ws <= L"ccc");
    EXPECT_TRUE(ws > L"bbb");
    EXPECT_TRUE(ws >= L"bbb");
    EXPECT_TRUE(ws >= L"ccc");
    EXPECT_TRUE(ws == mg::wstringref(L"ccc"));

    EXPECT_TRUE(L"ccc" == ws);
    EXPECT_TRUE(L"ddd" != ws);
    EXPECT_TRUE(L"bbb" < ws);
    EXPECT_TRUE(L"bbb" <= ws);
    EXPECT_TRUE(L"ccc" <= ws);
    EXPECT_TRUE(L"ddd" > ws);
    EXPECT_TRUE(L"ddd" >= ws);
    EXPECT_TRUE(L"ccc" >= ws);
    EXPECT_TRUE(mg::wstringref(L"ccc") == ws);
}

