#include "mgstringref_test.h"

TEST_F(StandardAllocator, EmptyConstrution)
{
    using namespace mg;
    stringref s;
    ustringref us;
    EXPECT_TRUE(s.empty());
    EXPECT_TRUE(us.empty());
}

TEST_F(CustomAllocator, EmptyConstrution)
{
    using namespace inplace;
    stringref s(a);
    ustringref us(a);
    EXPECT_EQ(a.used_block_count(), static_cast<std::size_t>(0));
    EXPECT_TRUE(s.empty());
    EXPECT_TRUE(us.empty());
}

TEST_F(StandardAllocator, ConstrutionFromConstString)
{
    using namespace mg;
    stringref s("Test string.");
    ustringref us(u"Test string.");
    EXPECT_EQ(s.size(), static_cast<std::size_t>(12));
    EXPECT_EQ(us.size(), static_cast<std::size_t>(12));

    stringref s1("Test string.", 4);
    ustringref us1(u"Test string.", 4);
    EXPECT_EQ(s1.size(), static_cast<std::size_t>(4));
    EXPECT_EQ(us1.size(), static_cast<std::size_t>(4));

    stringref s2("Test string.", 5, 6);
    ustringref us2(u"Test string.", 5, 6);
    EXPECT_EQ(s2.size(), static_cast<std::size_t>(6));
    EXPECT_EQ(us2.size(), static_cast<std::size_t>(6));

    stringref s4("Test", 5, 6);
    ustringref us4(u"Test", 5, 6);
    EXPECT_TRUE(s4.empty());
    EXPECT_TRUE(us4.empty());

    stringref s5("Test", 1, 6);
    ustringref us5(u"Test", 1, 6);
    EXPECT_EQ(s5.size(), static_cast<std::size_t>(3));
    EXPECT_EQ(us5.size(), static_cast<std::size_t>(3));
}

TEST_F(CustomAllocator, ConstrutionFromConstString)
{
    using namespace inplace;
    stringref s("Test string.", a);
    ustringref us(u"Test string.", a);
    EXPECT_EQ(a.used_block_count(), static_cast<std::size_t>(0));
    EXPECT_EQ(s.size(), static_cast<std::size_t>(12));
    EXPECT_EQ(us.size(), static_cast<std::size_t>(12));

    stringref s1("Test string.", 4, a);
    ustringref us1(u"Test string.", 4, a);
    EXPECT_EQ(a.used_block_count(), static_cast<std::size_t>(0));
    EXPECT_EQ(s1.size(), static_cast<std::size_t>(4));
    EXPECT_EQ(us1.size(), static_cast<std::size_t>(4));

    stringref s2("Test string.", 5, 6, a);
    ustringref us2(u"Test string.", 5, 6, a);
    EXPECT_EQ(a.used_block_count(), static_cast<std::size_t>(0));
    EXPECT_EQ(s2.size(), static_cast<std::size_t>(6));
    EXPECT_EQ(us2.size(), static_cast<std::size_t>(6));

    stringref s4("Test", 5, 6, a);
    ustringref us4(u"Test", 5, 6, a);
    EXPECT_EQ(a.used_block_count(), static_cast<std::size_t>(0));
    EXPECT_TRUE(s4.empty());
    EXPECT_TRUE(us4.empty());

    stringref s5("Test", 1, 6, a);
    ustringref us5(u"Test", 1, 6, a);
    EXPECT_EQ(a.used_block_count(), static_cast<std::size_t>(0));
    EXPECT_EQ(s5.size(), static_cast<std::size_t>(3));
    EXPECT_EQ(us5.size(), static_cast<std::size_t>(3));
}

TEST_F(StandardAllocator, ConstrutionFromStdString)
{
    std::string source("Test string");
    std::wstring wsource(L"Test string");

    using namespace mg;
    stringref s(source);
    wstringref ws(wsource);
    EXPECT_EQ(s.size(), source.size());
    EXPECT_EQ(ws.size(), wsource.size());

    stringref s1(source, 5, 6);
    wstringref ws1(wsource, 5, 6);
    EXPECT_EQ(s1.size(), static_cast<std::size_t>(6));
    EXPECT_EQ(ws1.size(), static_cast<std::size_t>(6));

    stringref s2(source, 5, 10);
    wstringref ws2(wsource, 5, 10);
    EXPECT_EQ(s2.size(), static_cast<std::size_t>(6));
    EXPECT_EQ(ws2.size(), static_cast<std::size_t>(6));

    stringref s3(source, 11, 4);
    wstringref ws3(wsource, 11, 4);
    EXPECT_TRUE(s3.empty());
    EXPECT_TRUE(ws3.empty());
}

TEST_F(CustomAllocator, ConstrutionFromStdString)
{
    std::string source("Test string");
    std::wstring wsource(L"Test string");

    using namespace inplace;
    stringref s(source, a);
    wstringref ws(wsource, a);
    EXPECT_EQ(a.used_block_count(), static_cast<std::size_t>(0));
    EXPECT_EQ(s.size(), source.size());
    EXPECT_EQ(ws.size(), wsource.size());

    stringref s1(source, 5, 6, a);
    wstringref ws1(wsource, 5, 6, a);
    EXPECT_EQ(a.used_block_count(), static_cast<std::size_t>(0));
    EXPECT_EQ(s1.size(), static_cast<std::size_t>(6));
    EXPECT_EQ(ws1.size(), static_cast<std::size_t>(6));

    stringref s2(source, 5, 10, a);
    wstringref ws2(wsource, 5, 10, a);
    EXPECT_EQ(a.used_block_count(), static_cast<std::size_t>(0));
    EXPECT_EQ(s2.size(), static_cast<std::size_t>(6));
    EXPECT_EQ(ws2.size(), static_cast<std::size_t>(6));

    stringref s3(source, 11, 4, a);
    wstringref ws3(wsource, 11, 4, a);
    EXPECT_EQ(a.used_block_count(), static_cast<std::size_t>(0));
    EXPECT_TRUE(s3.empty());
    EXPECT_TRUE(ws3.empty());
}

TEST_F(StandardAllocator, MoveConstrutionFromStdString)
{
    using namespace mg;
    stringref s(std::string("Test string"));
    wstringref ws(std::wstring(L"Test string"));
    EXPECT_EQ(s.size(), static_cast<std::size_t>(11));
    EXPECT_EQ(ws.size(), static_cast<std::size_t>(11));

    stringref sq(std::string{});
    wstringref wsq(std::wstring{});
    EXPECT_TRUE(sq.empty());
    EXPECT_TRUE(wsq.empty());

    stringref s2(std::string("Test string"), 5, 6);
    wstringref ws2(std::wstring(L"Test string"), 5, 6);
    EXPECT_EQ(s2.size(), static_cast<std::size_t>(6));
    EXPECT_EQ(ws2.size(), static_cast<std::size_t>(6));

    stringref s3(std::string("Test string"), 5, 10);
    wstringref ws3(std::wstring(L"Test string"), 5, 10);
    EXPECT_EQ(s3.size(), static_cast<std::size_t>(6));
    EXPECT_EQ(ws3.size(), static_cast<std::size_t>(6));

    stringref s4(std::string("Test string"), 11, 4);
    wstringref ws4(std::wstring(L"Test string"), 11, 4);
    EXPECT_TRUE(s4.empty());
    EXPECT_TRUE(ws4.empty());
}

TEST_F(CustomAllocator, MoveConstrutionFromStdString)
{
    using namespace inplace;
    stringref s(std::string("Test string"), a);
    wstringref ws(std::wstring(L"Test string"), a);
    EXPECT_EQ(a.used_block_count(), static_cast<std::size_t>(2));
    EXPECT_EQ(s.size(), static_cast<std::size_t>(11));
    EXPECT_EQ(ws.size(), static_cast<std::size_t>(11));

    stringref s1(std::string{}, a);
    wstringref ws1(std::wstring{}, a);
    EXPECT_EQ(a.used_block_count(), static_cast<std::size_t>(2));
    EXPECT_TRUE(s1.empty());
    EXPECT_TRUE(ws1.empty());

    stringref s2(std::string("Test string"), 5, 6, a);
    wstringref ws2(std::wstring(L"Test string"), 5, 6, a);
    EXPECT_EQ(a.used_block_count(), static_cast<std::size_t>(4));
    EXPECT_EQ(s2.size(), static_cast<std::size_t>(6));
    EXPECT_EQ(ws2.size(), static_cast<std::size_t>(6));

    stringref s3(std::string("Test string"), 5, 10, a);
    wstringref ws3(std::wstring(L"Test string"), 5, 10, a);
    EXPECT_EQ(a.used_block_count(), static_cast<std::size_t>(6));
    EXPECT_EQ(s3.size(), static_cast<std::size_t>(6));
    EXPECT_EQ(ws3.size(), static_cast<std::size_t>(6));

    stringref s4(std::string("Test string"), 11, 4, a);
    wstringref ws4(std::wstring(L"Test string"), 11, 4, a);
    EXPECT_EQ(a.used_block_count(), static_cast<std::size_t>(6));
    EXPECT_TRUE(s4.empty());
    EXPECT_TRUE(ws4.empty());
}

TEST_F(StandardAllocator, CopyConstruction)
{
    using namespace mg;
    stringref sref("ccc");
    wstringref wsref(L"ccc");

    stringref sref_copy1(sref);
    wstringref wsref_copy1(wsref);
    EXPECT_EQ(sref_copy1, "ccc");
    EXPECT_EQ(wsref_copy1, L"ccc");

    stringref sref_copy2(sref, 1, 1);
    wstringref wsref_copy2(wsref, 1, 1);
    EXPECT_EQ(sref_copy2, "c");
    EXPECT_EQ(wsref_copy2, L"c");

    stringref scopy(std::string("ccc"));
    wstringref wscopy(std::wstring(L"ccc"));

    stringref scopy_copy1(scopy);
    wstringref wscopy_copy1(wscopy);
    EXPECT_EQ(scopy_copy1, "ccc");
    EXPECT_EQ(wscopy_copy1, L"ccc");

    stringref scopy_copy2(scopy, 1, 1);
    wstringref wscopy_copy2(wscopy, 1, 1);
    EXPECT_EQ(scopy_copy2, "c");
    EXPECT_EQ(wscopy_copy2, L"c");

    stringref scopy_copy3(sref, 1, 1);
    wstringref wscopy_copy3(wsref, 1, 1);
    EXPECT_EQ(scopy_copy3, "c");
    EXPECT_EQ(wscopy_copy3, L"c");
}

TEST_F(CustomAllocator, CopyConstruction)
{
    using namespace inplace;
    stringref sref("ccc", a);
    wstringref wsref(L"ccc", a);
    EXPECT_EQ(a.used_block_count(), static_cast<std::size_t>(0));

    stringref sref_copy1(sref);
    wstringref wsref_copy1(wsref);
    EXPECT_EQ(a.used_block_count(), static_cast<std::size_t>(0));
    EXPECT_EQ(sref_copy1, "ccc");
    EXPECT_EQ(wsref_copy1, L"ccc");

    stringref sref_copy2(sref, 1, 1, a);
    wstringref wsref_copy2(wsref, 1, 1, a);
    EXPECT_EQ(a.used_block_count(), static_cast<std::size_t>(0));
    EXPECT_EQ(sref_copy2, "c");
    EXPECT_EQ(wsref_copy2, L"c");

    stringref scopy(std::string("ccc"), a);
    wstringref wscopy(std::wstring(L"ccc"), a);
    EXPECT_EQ(a.used_block_count(), static_cast<std::size_t>(2));

    stringref scopy_copy1(scopy);
    wstringref wscopy_copy1(wscopy);
    EXPECT_EQ(a.used_block_count(), static_cast<std::size_t>(2));
    EXPECT_EQ(scopy_copy1, "ccc");
    EXPECT_EQ(wscopy_copy1, L"ccc");

    stringref scopy_copy2(scopy, 1, 1, a);
    wstringref wscopy_copy2(wscopy, 1, 1, a);
    EXPECT_EQ(a.used_block_count(), static_cast<std::size_t>(4));
    EXPECT_EQ(scopy_copy2, "c");
    EXPECT_EQ(wscopy_copy2, L"c");

    mg::stringref scopy_copy3(scopy, 1, 1);
    mg::wstringref wscopy_copy3(wscopy, 1, 1);
    EXPECT_EQ(a.used_block_count(), static_cast<std::size_t>(4));
    EXPECT_EQ(scopy_copy3, "c");
    EXPECT_EQ(wscopy_copy3, L"c");

    stringref scopy_copy4(sref, 1, 1, a);
    wstringref wscopy_copy4(wsref, 1, 1, a);
    EXPECT_EQ(a.used_block_count(), static_cast<std::size_t>(4));
    EXPECT_EQ(scopy_copy4, "c");
    EXPECT_EQ(wscopy_copy4, L"c");
}

TEST_F(StandardAllocator, CopyConstructionWithOffsetAndLenght)
{
    using namespace mg;
    {
        stringref s(std::string("Test string"));
        wstringref ws(std::wstring(L"Test string"));

        stringref s1(s, 5, 6);
        wstringref ws1(ws, 5, 6);
        EXPECT_EQ(s1, "string");
        EXPECT_EQ(ws1, L"string");

        stringref s2(s, 5, 0);
        wstringref ws2(ws, 5, 0);
        EXPECT_TRUE(s2.empty());
        EXPECT_TRUE(ws2.empty());
    }

    stringref *s = new stringref(std::string("Test string"));
    wstringref *ws = new wstringref(std::wstring(L"Test string"));
    stringref s1(*s, 5, 6);
    wstringref ws1(*ws, 5, 6);
    delete s;
    delete ws;
    EXPECT_EQ(s1, "string");
    EXPECT_EQ(ws1, L"string");
}

TEST_F(CustomAllocator, CopyConstructionWithOffsetAndLenght)
{
    using namespace inplace;
    {
        stringref s(std::string("Test string"), a);
        wstringref ws(std::wstring(L"Test string"), a);
        EXPECT_EQ(a.used_block_count(), static_cast<std::size_t>(2));

        stringref s1(s, 5, 6);
        wstringref ws1(ws, 5, 6);
        EXPECT_EQ(a.used_block_count(), static_cast<std::size_t>(2));
        EXPECT_EQ(s1, "string");
        EXPECT_EQ(ws1, L"string");

        stringref s2(s, 5, 0);
        wstringref ws2(ws, 5, 0);
        EXPECT_EQ(a.used_block_count(), static_cast<std::size_t>(2));
        EXPECT_TRUE(s2.empty());
        EXPECT_TRUE(ws2.empty());
    }
    EXPECT_EQ(a.used_block_count(), static_cast<std::size_t>(0));

    stringref *s = new stringref(std::string("Test string"), a);
    wstringref *ws = new wstringref(std::wstring(L"Test string"), a);
    EXPECT_EQ(a.used_block_count(), static_cast<std::size_t>(2));

    stringref s1(*s, 5, 6);
    wstringref ws1(*ws, 5, 6);
    EXPECT_EQ(a.used_block_count(), static_cast<std::size_t>(2));
    delete s;
    delete ws;
    EXPECT_EQ(a.used_block_count(), static_cast<std::size_t>(2));
    EXPECT_EQ(s1, "string");
    EXPECT_EQ(ws1, L"string");
}

TEST_F(StandardAllocator, MoveConstruction)
{
    using namespace mg;
    stringref s1(stringref("Test string"));
    wstringref ws1(wstringref(L"Test string"));
    EXPECT_EQ(s1, "Test string");
    EXPECT_EQ(ws1, L"Test string");

    stringref s2(stringref(std::string("Test string")));
    wstringref ws2(wstringref(std::wstring(L"Test string")));
    EXPECT_EQ(s2, "Test string");
    EXPECT_EQ(ws2, L"Test string");

    stringref s3(stringref("Test string"), 5, 6);
    wstringref ws3(wstringref(L"Test string"), 5, 6);
    EXPECT_EQ(s3, "string");
    EXPECT_EQ(ws3, L"string");

    stringref s4(stringref("Test string"), 5, 0);
    wstringref ws4(wstringref(L"Test string"), 5, 0);
    EXPECT_EQ(s4, "");
    EXPECT_EQ(ws4, L"");

    stringref s5(stringref(std::string("Test string")), 5, 6);
    wstringref ws5(wstringref(std::wstring(L"Test string")), 5, 6);
    EXPECT_EQ(s5, "string");
    EXPECT_EQ(ws5, L"string");
}

TEST_F(CustomAllocator, MoveConstruction)
{
    using namespace inplace;
    a.clear_usage();
    stringref s1(stringref("Test string", a));
    wstringref ws1(wstringref(L"Test string", a));
    EXPECT_EQ(a.used_block_count(), static_cast<std::size_t>(0));
    EXPECT_EQ(a.alloc_count(), static_cast<std::size_t>(0));
    EXPECT_EQ(a.dealloc_count(), static_cast<std::size_t>(0));
    EXPECT_EQ(s1, "Test string");
    EXPECT_EQ(ws1, L"Test string");

    stringref s2(stringref(std::string("Test string"), a));
    wstringref ws2(wstringref(std::wstring(L"Test string"), a));
    EXPECT_EQ(a.used_block_count(), static_cast<std::size_t>(2));
    EXPECT_EQ(a.alloc_count(), static_cast<std::size_t>(2));
    EXPECT_EQ(a.dealloc_count(), static_cast<std::size_t>(0));
    EXPECT_EQ(s2, "Test string");
    EXPECT_EQ(ws2, L"Test string");

    stringref s3(stringref("Test string", a), 5, 6);
    wstringref ws3(wstringref(L"Test string", a), 5, 6);
    EXPECT_EQ(a.used_block_count(), static_cast<std::size_t>(2));
    EXPECT_EQ(a.alloc_count(), static_cast<std::size_t>(2));
    EXPECT_EQ(a.dealloc_count(), static_cast<std::size_t>(0));
    EXPECT_EQ(s3, "string");
    EXPECT_EQ(ws3, L"string");

    stringref s4(stringref("Test string", a), 5, 0);
    wstringref ws4(wstringref(L"Test string", a), 5, 0);
    EXPECT_EQ(a.used_block_count(), static_cast<std::size_t>(2));
    EXPECT_EQ(a.alloc_count(), static_cast<std::size_t>(2));
    EXPECT_EQ(a.dealloc_count(), static_cast<std::size_t>(0));
    EXPECT_EQ(s4, "");
    EXPECT_EQ(ws4, L"");

    stringref s5(stringref(std::string("Test string"), a), 5, 6);
    wstringref ws5(wstringref(std::wstring(L"Test string"), a), 5, 6);
    EXPECT_EQ(a.used_block_count(), static_cast<std::size_t>(4));
    EXPECT_EQ(a.alloc_count(), static_cast<std::size_t>(4));
    EXPECT_EQ(a.dealloc_count(), static_cast<std::size_t>(0));
    EXPECT_EQ(s5, "string");
    EXPECT_EQ(ws5, L"string");
}

TEST_F(StandardAllocator, DetachedConstructors)
{
    using namespace mg;
    stringref s1("Test string", stringref::detached);
    wstringref ws1(L"Test string", wstringref::detached);
    EXPECT_EQ(s1, "Test string");
    EXPECT_EQ(ws1, L"Test string");

    stringref s3("Test string", 4, stringref::detached);
    wstringref ws3(L"Test string", 4, wstringref::detached);
    EXPECT_EQ(s3, "Test");
    EXPECT_EQ(ws3, L"Test");

    stringref s4("Test string", 5, 6, stringref::detached);
    wstringref ws4(L"Test string", 5, 6, wstringref::detached);
    EXPECT_EQ(s4, "string");
    EXPECT_EQ(ws4, L"string");

    std::string ss("Test string");
    std::wstring wss(L"Test string");

    stringref s8(ss, stringref::detached);
    wstringref ws8(wss, wstringref::detached);
    EXPECT_EQ(s8, "Test string");
    EXPECT_EQ(ws8, L"Test string");

    stringref s9(ss, 5, 6, stringref::detached);
    wstringref ws9(wss, 5, 6, wstringref::detached);
    EXPECT_EQ(s9, "string");
    EXPECT_EQ(ws9, L"string");
}

TEST_F(CustomAllocator, DetachedConstructors)
{
    using namespace inplace;
    stringref s1("Test string", stringref::detached, a);
    wstringref ws1(L"Test string", wstringref::detached, a);
    EXPECT_EQ(a.used_block_count(), static_cast<std::size_t>(2));
    EXPECT_EQ(s1, "Test string");
    EXPECT_EQ(ws1, L"Test string");

    stringref s2("Test string", a);
    wstringref ws2(L"Test string", a);
    EXPECT_EQ(a.used_block_count(), static_cast<std::size_t>(2));
    EXPECT_EQ(s2, "Test string");
    EXPECT_EQ(ws2, L"Test string");

    stringref s3("Test string", 4, stringref::detached, a);
    wstringref ws3(L"Test string", 4, wstringref::detached, a);
    EXPECT_EQ(a.used_block_count(), static_cast<std::size_t>(4));
    EXPECT_EQ(s3, "Test");
    EXPECT_EQ(ws3, L"Test");

    stringref s4("Test string", 5, 6, stringref::detached, a);
    wstringref ws4(L"Test string", 5, 6, wstringref::detached, a);
    EXPECT_EQ(a.used_block_count(), static_cast<std::size_t>(6));
    EXPECT_EQ(s4, "string");
    EXPECT_EQ(ws4, L"string");

    stringref s5(s1, a2);
    wstringref ws5(ws1, a2);
    EXPECT_EQ(a.used_block_count(), static_cast<std::size_t>(6));
    EXPECT_EQ(a2.used_block_count(), static_cast<std::size_t>(2));
    EXPECT_EQ(s5, "Test string");
    EXPECT_EQ(ws5, L"Test string");

    stringref s6(s2, a2);
    wstringref ws6(ws2, a2);
    EXPECT_EQ(a.used_block_count(), static_cast<std::size_t>(6));
    EXPECT_EQ(a2.used_block_count(), static_cast<std::size_t>(2));
    EXPECT_EQ(s6, "Test string");
    EXPECT_EQ(ws6, L"Test string");

    std::string ss("Test string");
    std::wstring wss(L"Test string");

    stringref s7(ss, a2);
    wstringref ws7(wss, a2);
    EXPECT_EQ(a.used_block_count(), static_cast<std::size_t>(6));
    EXPECT_EQ(a2.used_block_count(), static_cast<std::size_t>(2));
    EXPECT_EQ(s7, "Test string");
    EXPECT_EQ(ws7, L"Test string");

    stringref s8(ss, stringref::detached, a2);
    wstringref ws8(wss, wstringref::detached, a2);
    EXPECT_EQ(a.used_block_count(), static_cast<std::size_t>(6));
    EXPECT_EQ(a2.used_block_count(), static_cast<std::size_t>(4));
    EXPECT_EQ(s8, "Test string");
    EXPECT_EQ(ws8, L"Test string");

    stringref s9(ss, 5, 6, stringref::detached, a2);
    wstringref ws9(wss, 5, 6, wstringref::detached, a2);
    EXPECT_EQ(a.used_block_count(), static_cast<std::size_t>(6));
    EXPECT_EQ(a2.used_block_count(), static_cast<std::size_t>(6));
    EXPECT_EQ(s9, "string");
    EXPECT_EQ(ws9, L"string");
}
