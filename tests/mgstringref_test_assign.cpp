#include "mgstringref_test.h"

TEST_F(StandardAllocator, AssingFromOthers)
{
    char str[] = "Test string";
    wchar_t wstr[] = L"Test string";
    std::string sstr("Test string");
    std::wstring wsstr(L"Test string");

    using namespace mg;
    stringref s;
    wstringref ws;

    s.assign(str);
    ws.assign(wstr);
    EXPECT_EQ(s, "Test string");
    EXPECT_EQ(ws, L"Test string");

    s.assign(str, stringref::detached);
    ws.assign(wstr, wstringref::detached);
    EXPECT_EQ(s, "Test string");
    EXPECT_EQ(ws, L"Test string");

    s.assign(str, 4);
    ws.assign(wstr, 4);
    EXPECT_EQ(s, "Test");
    EXPECT_EQ(ws, L"Test");

    s.assign(str, 4, stringref::detached);
    ws.assign(wstr, 4, stringref::detached);
    EXPECT_EQ(s, "Test");
    EXPECT_EQ(ws, L"Test");

    s.assign(str, 5, 6);
    ws.assign(wstr, 5, 6);
    EXPECT_EQ(s, "string");
    EXPECT_EQ(ws, L"string");

    s.assign(str, 5, 6, stringref::detached);
    ws.assign(wstr, 5, 6, stringref::detached);
    EXPECT_EQ(s, "string");
    EXPECT_EQ(ws, L"string");

    s.assign(str, 4, 1, 10);
    ws.assign(wstr, 4, 1, 10);
    EXPECT_EQ(s, "est");
    EXPECT_EQ(ws, L"est");

    s.assign(str, 4, 1, 10, stringref::detached);
    ws.assign(wstr, 4, 1, 10, wstringref::detached);
    EXPECT_EQ(s, "est");
    EXPECT_EQ(ws, L"est");

    s.assign(sstr);
    ws.assign(wsstr);
    EXPECT_EQ(s, "Test string");
    EXPECT_EQ(ws, L"Test string");

    s.assign(sstr, stringref::detached);
    ws.assign(wsstr, wstringref::detached);
    EXPECT_EQ(s, "Test string");
    EXPECT_EQ(ws, L"Test string");

    s.assign(sstr, 5, 6);
    ws.assign(wsstr, 5, 6);
    EXPECT_EQ(s, "string");
    EXPECT_EQ(ws, L"string");

    s.assign(sstr, 5, 6, stringref::detached);
    ws.assign(wsstr, 5, 6, stringref::detached);
    EXPECT_EQ(s, "string");
    EXPECT_EQ(ws, L"string");

    s.assign(std::string("Test string"));
    ws.assign(std::wstring(L"Test string"));
    EXPECT_EQ(s, "Test string");
    EXPECT_EQ(ws, L"Test string");

    s.assign(std::string("Test string"), 5, 6);
    ws.assign(std::wstring(L"Test string"), 5, 6);
    EXPECT_EQ(s, "string");
    EXPECT_EQ(ws, L"string");
}

TEST_F(CustomAllocator, AssingFromOthers)
{
    char str[] = "Test string";
    wchar_t wstr[] = L"Test string";
    std::string sstr("Test string");
    std::wstring wsstr(L"Test string");

    using namespace inplace;
    stringref s(a);
    wstringref ws(a2);

    s.assign(str);
    ws.assign(wstr);
    EXPECT_EQ(a.used_block_count(), static_cast<std::size_t>(0));
    EXPECT_EQ(a2.used_block_count(), static_cast<std::size_t>(0));
    EXPECT_EQ(s, "Test string");
    EXPECT_EQ(ws, L"Test string");

    s.assign(str, stringref::detached);
    ws.assign(wstr, wstringref::detached);
    EXPECT_EQ(a.used_block_count(), static_cast<std::size_t>(1));
    EXPECT_EQ(a2.used_block_count(), static_cast<std::size_t>(1));
    EXPECT_EQ(s, "Test string");
    EXPECT_EQ(ws, L"Test string");

    s.assign(str, 4);
    ws.assign(wstr, 4);
    EXPECT_EQ(a.used_block_count(), static_cast<std::size_t>(0));
    EXPECT_EQ(a2.used_block_count(), static_cast<std::size_t>(0));
    EXPECT_EQ(s, "Test");
    EXPECT_EQ(ws, L"Test");

    s.assign(str, 4, stringref::detached);
    ws.assign(wstr, 4, stringref::detached);
    EXPECT_EQ(a.used_block_count(), static_cast<std::size_t>(1));
    EXPECT_EQ(a2.used_block_count(), static_cast<std::size_t>(1));
    EXPECT_EQ(s, "Test");
    EXPECT_EQ(ws, L"Test");

    s.assign(str, 5, 6);
    ws.assign(wstr, 5, 6);
    EXPECT_EQ(a.used_block_count(), static_cast<std::size_t>(0));
    EXPECT_EQ(a2.used_block_count(), static_cast<std::size_t>(0));
    EXPECT_EQ(s, "string");
    EXPECT_EQ(ws, L"string");

    s.assign(str, 5, 6, stringref::detached);
    ws.assign(wstr, 5, 6, stringref::detached);
    EXPECT_EQ(a.used_block_count(), static_cast<std::size_t>(1));
    EXPECT_EQ(a2.used_block_count(), static_cast<std::size_t>(1));
    EXPECT_EQ(s, "string");
    EXPECT_EQ(ws, L"string");

    s.assign(str, 4, 1, 10);
    ws.assign(wstr, 4, 1, 10);
    EXPECT_EQ(a.used_block_count(), static_cast<std::size_t>(0));
    EXPECT_EQ(a2.used_block_count(), static_cast<std::size_t>(0));
    EXPECT_EQ(s, "est");
    EXPECT_EQ(ws, L"est");

    s.assign(str, 4, 1, 10, stringref::detached);
    ws.assign(wstr, 4, 1, 10, wstringref::detached);
    EXPECT_EQ(a.used_block_count(), static_cast<std::size_t>(1));
    EXPECT_EQ(a2.used_block_count(), static_cast<std::size_t>(1));
    EXPECT_EQ(s, "est");
    EXPECT_EQ(ws, L"est");

    s.assign(sstr);
    ws.assign(wsstr);
    EXPECT_EQ(a.used_block_count(), static_cast<std::size_t>(0));
    EXPECT_EQ(a2.used_block_count(), static_cast<std::size_t>(0));
    EXPECT_EQ(s, "Test string");
    EXPECT_EQ(ws, L"Test string");

    s.assign(sstr, stringref::detached);
    ws.assign(wsstr, wstringref::detached);
    EXPECT_EQ(a.used_block_count(), static_cast<std::size_t>(1));
    EXPECT_EQ(a2.used_block_count(), static_cast<std::size_t>(1));
    EXPECT_EQ(s, "Test string");
    EXPECT_EQ(ws, L"Test string");

    s.assign(sstr, 5, 6);
    ws.assign(wsstr, 5, 6);
    EXPECT_EQ(a.used_block_count(), static_cast<std::size_t>(0));
    EXPECT_EQ(a2.used_block_count(), static_cast<std::size_t>(0));
    EXPECT_EQ(s, "string");
    EXPECT_EQ(ws, L"string");

    s.assign(sstr, 5, 6, stringref::detached);
    ws.assign(wsstr, 5, 6, stringref::detached);
    EXPECT_EQ(a.used_block_count(), static_cast<std::size_t>(1));
    EXPECT_EQ(a2.used_block_count(), static_cast<std::size_t>(1));
    EXPECT_EQ(s, "string");
    EXPECT_EQ(ws, L"string");

    s.assign(std::string("Test string"));
    ws.assign(std::wstring(L"Test string"));
    EXPECT_EQ(a.used_block_count(), static_cast<std::size_t>(1));
    EXPECT_EQ(a2.used_block_count(), static_cast<std::size_t>(1));
    EXPECT_EQ(s, "Test string");
    EXPECT_EQ(ws, L"Test string");

    s.assign(std::string("Test string"), 5, 6);
    ws.assign(std::wstring(L"Test string"), 5, 6);
    EXPECT_EQ(a.used_block_count(), static_cast<std::size_t>(1));
    EXPECT_EQ(a2.used_block_count(), static_cast<std::size_t>(1));
    EXPECT_EQ(s, "string");
    EXPECT_EQ(ws, L"string");
}

TEST_F(StandardAllocator, AssingFromOthersEmpty)
{
    std::string strempty;
    std::wstring wstrempty;
    std::string str("Test");
    std::wstring wstr(L"Test");

    using namespace mg;
    stringref source("Test string", stringref::detached);
    wstringref wsource(L"Test string", wstringref::detached);

    stringref s01(source);
    wstringref ws01(wsource);
    s01.assign("");
    ws01.assign(L"");
    EXPECT_TRUE(s01.empty());
    EXPECT_TRUE(ws01.empty());

    stringref s02(source);
    wstringref ws02(wsource);
    s02.assign("", stringref::detached);
    ws02.assign(L"", wstringref::detached);
    EXPECT_TRUE(s02.empty());
    EXPECT_TRUE(ws02.empty());

    stringref s03(source);
    wstringref ws03(wsource);
    s03.assign("Test", 0);
    ws03.assign(L"Test", 0);
    EXPECT_TRUE(s03.empty());
    EXPECT_TRUE(ws03.empty());

    stringref s04(source);
    wstringref ws04(wsource);
    s04.assign("Test", 0, stringref::detached);
    ws04.assign(L"Test", 0, wstringref::detached);
    EXPECT_TRUE(s04.empty());
    EXPECT_TRUE(ws04.empty());

    stringref s05(source);
    wstringref ws05(wsource);
    s05.assign("Test", 5, 6);
    ws05.assign(L"Test", 5, 6);
    EXPECT_TRUE(s05.empty());
    EXPECT_TRUE(ws05.empty());

    stringref s06(source);
    wstringref ws06(wsource);
    s06.assign("Test", 5, 6, stringref::detached);
    ws06.assign(L"Test", 5, 6, wstringref::detached);
    EXPECT_TRUE(s06.empty());
    EXPECT_TRUE(ws06.empty());

    stringref s07(source);
    wstringref ws07(wsource);
    s07.assign("Test", 4, 5, 6);
    ws07.assign(L"Test", 4, 5, 6);
    EXPECT_TRUE(s07.empty());
    EXPECT_TRUE(ws07.empty());

    stringref s08(source);
    wstringref ws08(wsource);
    s08.assign("Test", 4, 5, 6, stringref::detached);
    ws08.assign(L"Test", 4, 5, 6, wstringref::detached);
    EXPECT_TRUE(s08.empty());
    EXPECT_TRUE(ws08.empty());

    stringref s09(source);
    wstringref ws09(wsource);
    s09.assign(strempty);
    ws09.assign(wstrempty);
    EXPECT_TRUE(s09.empty());
    EXPECT_TRUE(ws09.empty());

    stringref s10(source);
    wstringref ws10(wsource);
    s10.assign(strempty, stringref::detached);
    ws10.assign(wstrempty, wstringref::detached);
    EXPECT_TRUE(s10.empty());
    EXPECT_TRUE(ws10.empty());

    stringref s11(source);
    wstringref ws11(wsource);
    s11.assign(str, 5, 6);
    ws11.assign(wstr, 5, 6);
    EXPECT_TRUE(s11.empty());
    EXPECT_TRUE(ws11.empty());

    stringref s12(source);
    wstringref ws12(wsource);
    s12.assign(str, 5, 6, stringref::detached);
    ws12.assign(wstr, 5, 6, wstringref::detached);
    EXPECT_TRUE(s12.empty());
    EXPECT_TRUE(ws12.empty());

    stringref s13(source);
    wstringref ws13(wsource);
    s13.assign(std::string{});
    ws13.assign(std::wstring{});
    EXPECT_TRUE(s13.empty());
    EXPECT_TRUE(ws13.empty());

    stringref s14(source);
    wstringref ws14(wsource);
    s14.assign(std::string("Test"), 5, 6);
    ws14.assign(std::wstring(L"Test"), 5, 6);
    EXPECT_TRUE(s14.empty());
    EXPECT_TRUE(ws14.empty());
}

TEST_F(CustomAllocator, AssingFromOthersEmpty)
{
    std::string strempty;
    std::wstring wstrempty;
    std::string str("Test");
    std::wstring wstr(L"Test");

    using namespace inplace;
    stringref source("Test string", stringref::detached, a);
    wstringref wsource(L"Test string", wstringref::detached, a2);
    EXPECT_EQ(a.used_block_count(), static_cast<std::size_t>(1));
    EXPECT_EQ(a2.used_block_count(), static_cast<std::size_t>(1));

    stringref s01(source);
    wstringref ws01(wsource);
    s01.assign("");
    ws01.assign(L"");
    EXPECT_EQ(a.used_block_count(), static_cast<std::size_t>(1));
    EXPECT_EQ(a2.used_block_count(), static_cast<std::size_t>(1));
    EXPECT_TRUE(s01.empty());
    EXPECT_TRUE(ws01.empty());

    stringref s02(source);
    wstringref ws02(wsource);
    s02.assign("", stringref::detached);
    ws02.assign(L"", wstringref::detached);
    EXPECT_EQ(a.used_block_count(), static_cast<std::size_t>(1));
    EXPECT_EQ(a2.used_block_count(), static_cast<std::size_t>(1));
    EXPECT_TRUE(s02.empty());
    EXPECT_TRUE(ws02.empty());

    stringref s03(source);
    wstringref ws03(wsource);
    s03.assign("Test", 0);
    ws03.assign(L"Test", 0);
    EXPECT_EQ(a.used_block_count(), static_cast<std::size_t>(1));
    EXPECT_EQ(a2.used_block_count(), static_cast<std::size_t>(1));
    EXPECT_TRUE(s03.empty());
    EXPECT_TRUE(ws03.empty());

    stringref s04(source);
    wstringref ws04(wsource);
    s04.assign("Test", 0, stringref::detached);
    ws04.assign(L"Test", 0, wstringref::detached);
    EXPECT_EQ(a.used_block_count(), static_cast<std::size_t>(1));
    EXPECT_EQ(a2.used_block_count(), static_cast<std::size_t>(1));
    EXPECT_TRUE(s04.empty());
    EXPECT_TRUE(ws04.empty());

    stringref s05(source);
    wstringref ws05(wsource);
    s05.assign("Test", 5, 6);
    ws05.assign(L"Test", 5, 6);
    EXPECT_EQ(a.used_block_count(), static_cast<std::size_t>(1));
    EXPECT_EQ(a2.used_block_count(), static_cast<std::size_t>(1));
    EXPECT_TRUE(s05.empty());
    EXPECT_TRUE(ws05.empty());

    stringref s06(source);
    wstringref ws06(wsource);
    s06.assign("Test", 5, 6, stringref::detached);
    ws06.assign(L"Test", 5, 6, wstringref::detached);
    EXPECT_EQ(a.used_block_count(), static_cast<std::size_t>(1));
    EXPECT_EQ(a2.used_block_count(), static_cast<std::size_t>(1));
    EXPECT_TRUE(s06.empty());
    EXPECT_TRUE(ws06.empty());

    stringref s07(source);
    wstringref ws07(wsource);
    s07.assign("Test", 4, 5, 6);
    ws07.assign(L"Test", 4, 5, 6);
    EXPECT_EQ(a.used_block_count(), static_cast<std::size_t>(1));
    EXPECT_EQ(a2.used_block_count(), static_cast<std::size_t>(1));
    EXPECT_TRUE(s07.empty());
    EXPECT_TRUE(ws07.empty());

    stringref s08(source);
    wstringref ws08(wsource);
    s08.assign("Test", 4, 5, 6, stringref::detached);
    ws08.assign(L"Test", 4, 5, 6, wstringref::detached);
    EXPECT_EQ(a.used_block_count(), static_cast<std::size_t>(1));
    EXPECT_EQ(a2.used_block_count(), static_cast<std::size_t>(1));
    EXPECT_TRUE(s08.empty());
    EXPECT_TRUE(ws08.empty());

    stringref s09(source);
    wstringref ws09(wsource);
    s09.assign(strempty);
    ws09.assign(wstrempty);
    EXPECT_EQ(a.used_block_count(), static_cast<std::size_t>(1));
    EXPECT_EQ(a2.used_block_count(), static_cast<std::size_t>(1));
    EXPECT_TRUE(s09.empty());
    EXPECT_TRUE(ws09.empty());

    stringref s10(source);
    wstringref ws10(wsource);
    s10.assign(strempty, stringref::detached);
    ws10.assign(wstrempty, wstringref::detached);
    EXPECT_EQ(a.used_block_count(), static_cast<std::size_t>(1));
    EXPECT_EQ(a2.used_block_count(), static_cast<std::size_t>(1));
    EXPECT_TRUE(s10.empty());
    EXPECT_TRUE(ws10.empty());

    stringref s11(source);
    wstringref ws11(wsource);
    s11.assign(str, 5, 6);
    ws11.assign(wstr, 5, 6);
    EXPECT_EQ(a.used_block_count(), static_cast<std::size_t>(1));
    EXPECT_EQ(a2.used_block_count(), static_cast<std::size_t>(1));
    EXPECT_TRUE(s11.empty());
    EXPECT_TRUE(ws11.empty());

    stringref s12(source);
    wstringref ws12(wsource);
    s12.assign(str, 5, 6, stringref::detached);
    ws12.assign(wstr, 5, 6, wstringref::detached);
    EXPECT_EQ(a.used_block_count(), static_cast<std::size_t>(1));
    EXPECT_EQ(a2.used_block_count(), static_cast<std::size_t>(1));
    EXPECT_TRUE(s12.empty());
    EXPECT_TRUE(ws12.empty());

    stringref s13(source);
    wstringref ws13(wsource);
    s13.assign(std::string{});
    ws13.assign(std::wstring{});
    EXPECT_EQ(a.used_block_count(), static_cast<std::size_t>(1));
    EXPECT_EQ(a2.used_block_count(), static_cast<std::size_t>(1));
    EXPECT_TRUE(s13.empty());
    EXPECT_TRUE(ws13.empty());

    stringref s14(source);
    wstringref ws14(wsource);
    s14.assign(std::string("Test"), 5, 6);
    ws14.assign(std::wstring(L"Test"), 5, 6);
    EXPECT_EQ(a.used_block_count(), static_cast<std::size_t>(1));
    EXPECT_EQ(a2.used_block_count(), static_cast<std::size_t>(1));
    EXPECT_TRUE(s14.empty());
    EXPECT_TRUE(ws14.empty());
}

TEST_F(CustomAllocator, AssingFromSelfOtherAllocator)
{
    mg::stringref sref("Test string");
    mg::wstringref wsref(L"Test string");
    mg::stringref scopy("Test string", mg::stringref::detached);
    mg::wstringref wscopy(L"Test string", mg::wstringref::detached);
    mg::stringref sempty;
    mg::wstringref wsempty;
    EXPECT_EQ(a.used_block_count(), static_cast<std::size_t>(0));
    EXPECT_EQ(a2.used_block_count(), static_cast<std::size_t>(0));

    using namespace inplace;

//    template<typename _OTraits, typename _OAlloc>
//    inline basic_stringref& assign(const basic_stringref<value_type, _OTraits, _OAlloc>& other)
    stringref s01(a);
    wstringref ws01(a2);
    s01.assign(sref);
    ws01.assign(wsref);
    EXPECT_EQ(a.used_block_count(), static_cast<std::size_t>(0));
    EXPECT_EQ(a2.used_block_count(), static_cast<std::size_t>(0));
    EXPECT_EQ(s01, "Test string");
    EXPECT_EQ(ws01, L"Test string");
    s01.assign(scopy);
    ws01.assign(wscopy);
    EXPECT_EQ(a.used_block_count(), static_cast<std::size_t>(1));
    EXPECT_EQ(a2.used_block_count(), static_cast<std::size_t>(1));
    EXPECT_EQ(s01, "Test string");
    EXPECT_EQ(ws01, L"Test string");
    s01.assign(sempty);
    ws01.assign(wsempty);
    EXPECT_EQ(a.used_block_count(), static_cast<std::size_t>(0));
    EXPECT_EQ(a2.used_block_count(), static_cast<std::size_t>(0));
    EXPECT_TRUE(s01.empty());
    EXPECT_TRUE(ws01.empty());


//    template<typename _OTraits, typename _OAlloc>
//    inline basic_stringref& assign(const basic_stringref<value_type, _OTraits, _OAlloc>& other, std::true_type)
    stringref s02(a);
    wstringref ws02(a2);
    s02.assign(sref, stringref::detached);
    ws02.assign(wsref, wstringref::detached);
    EXPECT_EQ(a.used_block_count(), static_cast<std::size_t>(1));
    EXPECT_EQ(a2.used_block_count(), static_cast<std::size_t>(1));
    EXPECT_EQ(s02, "Test string");
    EXPECT_EQ(ws02, L"Test string");
    s02.assign(scopy, stringref::detached);
    ws02.assign(wscopy, wstringref::detached);
    EXPECT_EQ(a.used_block_count(), static_cast<std::size_t>(1));
    EXPECT_EQ(a2.used_block_count(), static_cast<std::size_t>(1));
    EXPECT_EQ(s02, "Test string");
    EXPECT_EQ(ws02, L"Test string");
    s02.assign(sempty, stringref::detached);
    ws02.assign(wsempty, wstringref::detached);
    EXPECT_EQ(a.used_block_count(), static_cast<std::size_t>(0));
    EXPECT_EQ(a2.used_block_count(), static_cast<std::size_t>(0));
    EXPECT_TRUE(s02.empty());
    EXPECT_TRUE(ws02.empty());

//    template<typename _OTraits, typename _OAlloc>
//    inline basic_stringref& assign(const basic_stringref<value_type, _OTraits, _OAlloc>& other, size_type offset,
//                                   size_type length)
    stringref s03(a);
    wstringref ws03(a2);
    s03.assign(sref, 5, 6);
    ws03.assign(wsref, 5, 6);
    EXPECT_EQ(a.used_block_count(), static_cast<std::size_t>(0));
    EXPECT_EQ(a2.used_block_count(), static_cast<std::size_t>(0));
    EXPECT_EQ(s03, "string");
    EXPECT_EQ(ws03, L"string");
    s03.assign(scopy, 5, 6);
    ws03.assign(wscopy, 5, 6);
    EXPECT_EQ(a.used_block_count(), static_cast<std::size_t>(1));
    EXPECT_EQ(a2.used_block_count(), static_cast<std::size_t>(1));
    EXPECT_EQ(s03, "string");
    EXPECT_EQ(ws03, L"string");
    s03.assign(scopy, 5, 100);
    ws03.assign(wscopy, 5, 100);
    EXPECT_EQ(a.used_block_count(), static_cast<std::size_t>(1));
    EXPECT_EQ(a2.used_block_count(), static_cast<std::size_t>(1));
    EXPECT_EQ(s03, "string");
    EXPECT_EQ(ws03, L"string");
    s03.assign(scopy, 20, 6);
    ws03.assign(wscopy, 20, 6);
    EXPECT_EQ(a.used_block_count(), static_cast<std::size_t>(0));
    EXPECT_EQ(a2.used_block_count(), static_cast<std::size_t>(0));
    EXPECT_TRUE(s03.empty());
    EXPECT_TRUE(ws03.empty());
    s03.assign(sempty);
    ws03.assign(wsempty);
    EXPECT_EQ(a.used_block_count(), static_cast<std::size_t>(0));
    EXPECT_EQ(a2.used_block_count(), static_cast<std::size_t>(0));
    EXPECT_TRUE(s03.empty());
    EXPECT_TRUE(ws03.empty());

//    template<typename _OTraits, typename _OAlloc>
//    inline basic_stringref& assign(const basic_stringref<value_type, _OTraits, _OAlloc>& other, size_type offset,
//                                   size_type length, std::true_type)
    stringref s04(a);
    wstringref ws04(a2);
    s04.assign(sref, 5, 6, stringref::detached);
    ws04.assign(wsref, 5, 6, wstringref::detached);
    EXPECT_EQ(a.used_block_count(), static_cast<std::size_t>(1));
    EXPECT_EQ(a2.used_block_count(), static_cast<std::size_t>(1));
    EXPECT_EQ(s04, "string");
    EXPECT_EQ(ws04, L"string");
    s04.assign(scopy, 5, 6, stringref::detached);
    ws04.assign(wscopy, 5, 6, wstringref::detached);
    EXPECT_EQ(a.used_block_count(), static_cast<std::size_t>(1));
    EXPECT_EQ(a2.used_block_count(), static_cast<std::size_t>(1));
    EXPECT_EQ(s04, "string");
    EXPECT_EQ(ws04, L"string");
    s04.assign(scopy, 5, 100, stringref::detached);
    ws04.assign(wscopy, 5, 100, wstringref::detached);
    EXPECT_EQ(a.used_block_count(), static_cast<std::size_t>(1));
    EXPECT_EQ(a2.used_block_count(), static_cast<std::size_t>(1));
    EXPECT_EQ(s04, "string");
    EXPECT_EQ(ws04, L"string");
    s04.assign(scopy, 20, 6, wstringref::detached);
    ws04.assign(wscopy, 20, 6, wstringref::detached);
    EXPECT_EQ(a.used_block_count(), static_cast<std::size_t>(0));
    EXPECT_EQ(a2.used_block_count(), static_cast<std::size_t>(0));
    EXPECT_TRUE(s04.empty());
    EXPECT_TRUE(ws04.empty());
    s04.assign(sempty, stringref::detached);
    ws04.assign(wsempty, wstringref::detached);
    EXPECT_EQ(a.used_block_count(), static_cast<std::size_t>(0));
    EXPECT_EQ(a2.used_block_count(), static_cast<std::size_t>(0));
    EXPECT_TRUE(s04.empty());
    EXPECT_TRUE(ws04.empty());
}

TEST_F(StandardAllocator, CopyAssingFromSelf)
{
    using namespace mg;
    stringref sref("Test string");
    wstringref wsref(L"Test string");
    stringref scopy("Test string", stringref::detached);
    wstringref wscopy(L"Test string", wstringref::detached);
    stringref sempty;
    wstringref wsempty;

//    basic_stringref& assign(const basic_stringref& other);
    stringref s01;
    wstringref ws01;
    EXPECT_EQ(s01.assign(sref), "Test string");
    EXPECT_EQ(ws01.assign(wsref), L"Test string");
    EXPECT_EQ(s01.assign(scopy), "Test string");
    EXPECT_EQ(ws01.assign(wscopy), L"Test string");
    EXPECT_TRUE(s01.assign(sempty).empty());
    EXPECT_TRUE(ws01.assign(wsempty).empty());

//    basic_stringref& assign(const basic_stringref& other, std::true_type);
    stringref s02;
    wstringref ws02;
    EXPECT_EQ(s02.assign(sref, stringref::detached), "Test string");
    EXPECT_EQ(ws02.assign(wsref, wstringref::detached), L"Test string");
    EXPECT_EQ(s02.assign(scopy, stringref::detached), "Test string");
    EXPECT_EQ(ws02.assign(wscopy, wstringref::detached), L"Test string");
    EXPECT_TRUE(s02.assign(sempty).empty());
    EXPECT_TRUE(ws02.assign(wsempty).empty());

//    basic_stringref& assign(const basic_stringref& other, size_type offset, size_type length);
    stringref s03;
    wstringref ws03;
    EXPECT_EQ(s03.assign(sref, 5, 6), "string");
    EXPECT_EQ(ws03.assign(wsref, 5, 6), L"string");
    EXPECT_EQ(s03.assign(scopy, 5, 6), "string");
    EXPECT_EQ(ws03.assign(wscopy, 5, 6), L"string");
    EXPECT_TRUE(s03.assign(scopy, 12, 5).empty());
    EXPECT_TRUE(ws03.assign(wscopy, 12, 5).empty());

//    basic_stringref& assign(const basic_stringref& other, size_type offset, size_type length, std::true_type);
    stringref s04;
    wstringref ws04;
    EXPECT_EQ(s04.assign(sref, 5, 6, stringref::detached), "string");
    EXPECT_EQ(ws04.assign(wsref, 5, 6, wstringref::detached), L"string");
    EXPECT_EQ(s04.assign(scopy, 5, 6, stringref::detached), "string");
    EXPECT_EQ(ws04.assign(wscopy, 5, 6, wstringref::detached), L"string");
    EXPECT_TRUE(s04.assign(scopy, 12, 5, stringref::detached).empty());
    EXPECT_TRUE(ws04.assign(wscopy, 12, 5, wstringref::detached).empty());
}

TEST_F(CustomAllocator, CopyAssingFromSelf)
{
    using namespace inplace;
    stringref sref("Test string", a);
    wstringref wsref(L"Test string", a2);
    stringref scopy("Test string", stringref::detached, a);
    wstringref wscopy(L"Test string", wstringref::detached, a2);
    stringref sempty(a);
    wstringref wsempty(a2);
    EXPECT_EQ(a.used_block_count(), static_cast<std::size_t>(1));
    EXPECT_EQ(a2.used_block_count(), static_cast<std::size_t>(1));

//    basic_stringref& assign(const basic_stringref& other);
    a.clear_usage();
    a2.clear_usage();
    stringref s01(a);
    wstringref ws01(a2);
    EXPECT_EQ(s01.assign(sref), "Test string");
    EXPECT_EQ(ws01.assign(wsref), L"Test string");
    EXPECT_EQ(a.used_block_count(), static_cast<std::size_t>(1));
    EXPECT_EQ(a2.used_block_count(), static_cast<std::size_t>(1));
    EXPECT_EQ(a.alloc_count(), static_cast<std::size_t>(0));
    EXPECT_EQ(a2.alloc_count(), static_cast<std::size_t>(0));
    EXPECT_EQ(s01.assign(scopy), "Test string");
    EXPECT_EQ(ws01.assign(wscopy), L"Test string");
    EXPECT_EQ(a.used_block_count(), static_cast<std::size_t>(1));
    EXPECT_EQ(a2.used_block_count(), static_cast<std::size_t>(1));
    EXPECT_EQ(a.alloc_count(), static_cast<std::size_t>(0));
    EXPECT_EQ(a2.alloc_count(), static_cast<std::size_t>(0));
    EXPECT_TRUE(s01.assign(sempty).empty());
    EXPECT_TRUE(ws01.assign(wsempty).empty());
    EXPECT_EQ(a.used_block_count(), static_cast<std::size_t>(1));
    EXPECT_EQ(a2.used_block_count(), static_cast<std::size_t>(1));
    EXPECT_EQ(a.alloc_count(), static_cast<std::size_t>(0));
    EXPECT_EQ(a2.alloc_count(), static_cast<std::size_t>(0));

//    basic_stringref& assign(const basic_stringref& other, std::true_type);
    a.clear_usage();
    a2.clear_usage();
    stringref s02(a);
    wstringref ws02(a2);
    EXPECT_EQ(s02.assign(sref, stringref::detached), "Test string");
    EXPECT_EQ(ws02.assign(wsref, wstringref::detached), L"Test string");
    EXPECT_EQ(a.used_block_count(), static_cast<std::size_t>(2));
    EXPECT_EQ(a2.used_block_count(), static_cast<std::size_t>(2));
    EXPECT_EQ(a.alloc_count(), static_cast<std::size_t>(1));
    EXPECT_EQ(a2.alloc_count(), static_cast<std::size_t>(1));
    EXPECT_EQ(s02.assign(scopy, stringref::detached), "Test string");
    EXPECT_EQ(ws02.assign(wscopy, wstringref::detached), L"Test string");
    EXPECT_EQ(a.used_block_count(), static_cast<std::size_t>(2));
    EXPECT_EQ(a2.used_block_count(), static_cast<std::size_t>(2));
    EXPECT_EQ(a.alloc_count(), static_cast<std::size_t>(2));
    EXPECT_EQ(a2.alloc_count(), static_cast<std::size_t>(2));
    EXPECT_EQ(a.dealloc_count(), static_cast<std::size_t>(1));
    EXPECT_EQ(a2.dealloc_count(), static_cast<std::size_t>(1));
    EXPECT_TRUE(s02.assign(sempty).empty());
    EXPECT_TRUE(ws02.assign(wsempty).empty());
    EXPECT_EQ(a.used_block_count(), static_cast<std::size_t>(1));
    EXPECT_EQ(a2.used_block_count(), static_cast<std::size_t>(1));
    EXPECT_EQ(a.alloc_count(), static_cast<std::size_t>(2));
    EXPECT_EQ(a2.alloc_count(), static_cast<std::size_t>(2));
    EXPECT_EQ(a.dealloc_count(), static_cast<std::size_t>(2));
    EXPECT_EQ(a2.dealloc_count(), static_cast<std::size_t>(2));

//    basic_stringref& assign(const basic_stringref& other, size_type offset, size_type length);
    a.clear_usage();
    a2.clear_usage();
    stringref s03(a);
    wstringref ws03(a2);
    EXPECT_EQ(s03.assign(sref, 5, 6), "string");
    EXPECT_EQ(ws03.assign(wsref, 5, 6), L"string");
    EXPECT_EQ(a.used_block_count(), static_cast<std::size_t>(1));
    EXPECT_EQ(a2.used_block_count(), static_cast<std::size_t>(1));
    EXPECT_EQ(a.alloc_count(), static_cast<std::size_t>(0));
    EXPECT_EQ(a2.alloc_count(), static_cast<std::size_t>(0));
    EXPECT_EQ(s03.assign(scopy, 5, 6), "string");
    EXPECT_EQ(ws03.assign(wscopy, 5, 6), L"string");
    EXPECT_EQ(a.used_block_count(), static_cast<std::size_t>(1));
    EXPECT_EQ(a2.used_block_count(), static_cast<std::size_t>(1));
    EXPECT_EQ(a.alloc_count(), static_cast<std::size_t>(0));
    EXPECT_EQ(a2.alloc_count(), static_cast<std::size_t>(0));
    EXPECT_TRUE(s03.assign(scopy, 12, 5).empty());
    EXPECT_TRUE(ws03.assign(wscopy, 12, 5).empty());
    EXPECT_EQ(a.used_block_count(), static_cast<std::size_t>(1));
    EXPECT_EQ(a2.used_block_count(), static_cast<std::size_t>(1));
    EXPECT_EQ(a.alloc_count(), static_cast<std::size_t>(0));
    EXPECT_EQ(a2.alloc_count(), static_cast<std::size_t>(0));

//    basic_stringref& assign(const basic_stringref& other, size_type offset, size_type length, std::true_type);
    a.clear_usage();
    a2.clear_usage();
    stringref s04(a);
    wstringref ws04(a2);
    EXPECT_EQ(s04.assign(sref, 5, 6, stringref::detached), "string");
    EXPECT_EQ(ws04.assign(wsref, 5, 6, wstringref::detached), L"string");
    EXPECT_EQ(a.used_block_count(), static_cast<std::size_t>(2));
    EXPECT_EQ(a2.used_block_count(), static_cast<std::size_t>(2));
    EXPECT_EQ(a.alloc_count(), static_cast<std::size_t>(1));
    EXPECT_EQ(a2.alloc_count(), static_cast<std::size_t>(1));
    EXPECT_EQ(s04.assign(scopy, 5, 6, stringref::detached), "string");
    EXPECT_EQ(ws04.assign(wscopy, 5, 6, wstringref::detached), L"string");
    EXPECT_EQ(a.used_block_count(), static_cast<std::size_t>(2));
    EXPECT_EQ(a2.used_block_count(), static_cast<std::size_t>(2));
    EXPECT_EQ(a.alloc_count(), static_cast<std::size_t>(2));
    EXPECT_EQ(a2.alloc_count(), static_cast<std::size_t>(2));
    EXPECT_EQ(a.dealloc_count(), static_cast<std::size_t>(1));
    EXPECT_EQ(a2.dealloc_count(), static_cast<std::size_t>(1));
    EXPECT_TRUE(s04.assign(scopy, 12, 5, stringref::detached).empty());
    EXPECT_TRUE(ws04.assign(wscopy, 12, 5, wstringref::detached).empty());
    EXPECT_EQ(a.used_block_count(), static_cast<std::size_t>(1));
    EXPECT_EQ(a2.used_block_count(), static_cast<std::size_t>(1));
    EXPECT_EQ(a.alloc_count(), static_cast<std::size_t>(2));
    EXPECT_EQ(a2.alloc_count(), static_cast<std::size_t>(2));
    EXPECT_EQ(a.dealloc_count(), static_cast<std::size_t>(2));
    EXPECT_EQ(a2.dealloc_count(), static_cast<std::size_t>(2));
}
