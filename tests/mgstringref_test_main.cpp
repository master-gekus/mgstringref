#include "mgstringref_test.h"

#include <cstdio>
#include <clocale>
#include <map>
#include <utility>

int main(int argc, char** argv)
{
#if defined(__MINGW32__)
    std::setlocale(LC_ALL, "Russian");
#elif defined(_WIN32)
    std::setlocale(LC_ALL, "ru-RU");
#else
    std::setlocale(LC_ALL, "ru_RU.UTF-8");
#endif

    ::testing::InitGoogleTest(&argc, argv);

    printf("Enviroment:\n"
           "  sizeof(char)     = %u\n"
           "  sizeof(char16_t) = %u\n"
           "  sizeof(wchar_t)  = %u\n",
           static_cast<unsigned>(sizeof(char)),
           static_cast<unsigned>(sizeof(char16_t)),
           static_cast<unsigned>(sizeof(wchar_t)));

    return RUN_ALL_TESTS();
}

CustomAllocator::CustomAllocator() :
    a(buf, sizeof(buf)), a2(buf2, sizeof(buf2))
{

}
void CustomAllocator::TearDown()
{
    EXPECT_EQ(a.used_block_count(), static_cast<std::size_t>(0));
    EXPECT_EQ(a2.used_block_count(), static_cast<std::size_t>(0));
}

TEST_F(CustomAllocator, StandardContainers)
{
    using namespace inplace;
    a.clear_usage();
    string *s = new string("String must be lenght enough to allocated memory.", a);
    string s1(*s);
    string s2 = *s;
    EXPECT_EQ(a.used_block_count(), static_cast<std::size_t>(3));
    EXPECT_EQ(a.alloc_count(), static_cast<std::size_t>(3));
    EXPECT_EQ(a.dealloc_count(), static_cast<std::size_t>(0));
    delete s;
    EXPECT_EQ(s1, s2);
    EXPECT_EQ(a.used_block_count(), static_cast<std::size_t>(2));
    EXPECT_EQ(a.alloc_count(), static_cast<std::size_t>(3));
    EXPECT_EQ(a.dealloc_count(), static_cast<std::size_t>(1));

    string s3(s1, a2);
    EXPECT_EQ(a.used_block_count(), static_cast<std::size_t>(2));
    EXPECT_EQ(a2.used_block_count(), static_cast<std::size_t>(1));
    EXPECT_STREQ(s3.c_str(), "String must be lenght enough to allocated memory.");

    std::map<string, string, std::less<string>, allocator<std::map<string, string>::value_type> > m(a);
    m.emplace(string("String must be lenght enough to allocated memory.", a),
              string("String must be lenght enough to allocated memory.", a));

    EXPECT_EQ(m.size(), static_cast<std::size_t>(1));
    EXPECT_EQ(m.begin()->first, s1);
}

TEST_F(StandardAllocator, Detach)
{
    char str[] = "Test string";
    wchar_t wstr[] = L"Test string";

    using namespace mg;

    stringref s1(str);
    stringref s2(str, 4);
    stringref s3(str, 5, 6);
    wstringref ws1(wstr);
    wstringref ws2(wstr, 4);
    wstringref ws3(wstr, 5, 6);
    EXPECT_EQ(s1, "Test string");
    EXPECT_EQ(s2, "Test");
    EXPECT_EQ(s3, "string");
    EXPECT_EQ(ws1, L"Test string");
    EXPECT_EQ(ws2, L"Test");
    EXPECT_EQ(ws3, L"string");
    EXPECT_FALSE(s1.is_detached());
    EXPECT_FALSE(s2.is_detached());
    EXPECT_FALSE(s3.is_detached());
    EXPECT_FALSE(ws1.is_detached());
    EXPECT_FALSE(ws2.is_detached());
    EXPECT_FALSE(ws3.is_detached());

    str[5] = 'S';
    wstr[5] = L'S';
    EXPECT_EQ(s1, "Test String");
    EXPECT_EQ(s2, "Test");
    EXPECT_EQ(s3, "String");
    EXPECT_EQ(ws1, L"Test String");
    EXPECT_EQ(ws2, L"Test");
    EXPECT_EQ(ws3, L"String");
    s1.detach();
    s2.detach();
    s3.detach();
    ws1.detach();
    ws2.detach();
    ws3.detach();
    EXPECT_TRUE(s1.is_detached());
    EXPECT_TRUE(s2.is_detached());
    EXPECT_TRUE(s3.is_detached());
    EXPECT_TRUE(ws1.is_detached());
    EXPECT_TRUE(ws2.is_detached());
    EXPECT_TRUE(ws3.is_detached());

    memset(str, 0, sizeof(str));
    memset(wstr, 0, sizeof(wstr));
    EXPECT_STREQ(str, "");
    EXPECT_STREQ(wstr, L"");

    EXPECT_EQ(s1, "Test String");
    EXPECT_EQ(s2, "Test");
    EXPECT_EQ(s3, "String");
    EXPECT_EQ(ws1, L"Test String");
    EXPECT_EQ(ws2, L"Test");
    EXPECT_EQ(ws3, L"String");
}

TEST_F(CustomAllocator, Detach)
{
    char str[] = "Test string";
    wchar_t wstr[] = L"Test string";

    using namespace inplace;

    stringref s1(str, a);
    stringref s2(str, 4, a2);
    stringref s3(str, 5, 6, a2);
    wstringref ws1(wstr, a);
    wstringref ws2(wstr, 4, a2);
    wstringref ws3(wstr, 5, 6, a2);
    EXPECT_EQ(a.used_block_count(), static_cast<std::size_t>(0));
    EXPECT_EQ(a2.used_block_count(), static_cast<std::size_t>(0));
    EXPECT_EQ(s1, "Test string");
    EXPECT_EQ(s2, "Test");
    EXPECT_EQ(s3, "string");
    EXPECT_EQ(ws1, L"Test string");
    EXPECT_EQ(ws2, L"Test");
    EXPECT_EQ(ws3, L"string");
    EXPECT_FALSE(s1.is_detached());
    EXPECT_FALSE(s2.is_detached());
    EXPECT_FALSE(s3.is_detached());
    EXPECT_FALSE(ws1.is_detached());
    EXPECT_FALSE(ws2.is_detached());
    EXPECT_FALSE(ws3.is_detached());

    str[5] = 'S';
    wstr[5] = L'S';
    EXPECT_EQ(s1, "Test String");
    EXPECT_EQ(s2, "Test");
    EXPECT_EQ(s3, "String");
    EXPECT_EQ(ws1, L"Test String");
    EXPECT_EQ(ws2, L"Test");
    EXPECT_EQ(ws3, L"String");
    s1.detach();
    s2.detach();
    s3.detach();
    ws1.detach();
    ws2.detach();
    ws3.detach();
    EXPECT_EQ(a.used_block_count(), static_cast<std::size_t>(2));
    EXPECT_EQ(a2.used_block_count(), static_cast<std::size_t>(4));
    EXPECT_TRUE(s1.is_detached());
    EXPECT_TRUE(s2.is_detached());
    EXPECT_TRUE(s3.is_detached());
    EXPECT_TRUE(ws1.is_detached());
    EXPECT_TRUE(ws2.is_detached());
    EXPECT_TRUE(ws3.is_detached());

    memset(str, 0, sizeof(str));
    memset(wstr, 0, sizeof(wstr));
    EXPECT_STREQ(str, "");
    EXPECT_STREQ(wstr, L"");

    EXPECT_EQ(s1, "Test String");
    EXPECT_EQ(s2, "Test");
    EXPECT_EQ(s3, "String");
    EXPECT_EQ(ws1, L"Test String");
    EXPECT_EQ(ws2, L"Test");
    EXPECT_EQ(ws3, L"String");
}

TEST(Common, CiCharTraits)
{
    using namespace mg;
    EXPECT_TRUE(ci_char_traits<char>::eq('a', 'a'));
    EXPECT_TRUE(ci_char_traits<char>::eq('A', 'a'));
    EXPECT_TRUE(ci_char_traits<char>::eq('a', 'A'));

    EXPECT_TRUE(ci_char_traits<char>::eq_int_type('a', 'a'));
    EXPECT_TRUE(ci_char_traits<char>::eq_int_type('A', 'a'));
    EXPECT_TRUE(ci_char_traits<char>::eq_int_type('a', 'A'));

    EXPECT_TRUE(ci_char_traits<char>::lt('a', 'b'));
    EXPECT_TRUE(ci_char_traits<char>::lt('A', 'b'));
    EXPECT_TRUE(ci_char_traits<char>::lt('a', 'B'));

    EXPECT_EQ(0, ci_char_traits<char>::compare("test", "test", 4));
    EXPECT_EQ(0, ci_char_traits<char>::compare("test", "TEST", 4));
    EXPECT_EQ(0, ci_char_traits<char>::compare("TEST", "test", 4));

    EXPECT_EQ(0, ci_char_traits<char>::compare("testA", "testD", 4));
    EXPECT_EQ(0, ci_char_traits<char>::compare("testB", "TESTF", 4));
    EXPECT_EQ(0, ci_char_traits<char>::compare("TESTC", "testG", 4));

    EXPECT_GT(0, ci_char_traits<char>::compare("abcd", "efgh", 4));
    EXPECT_GT(0, ci_char_traits<char>::compare("abcd", "EFGH", 4));
    EXPECT_GT(0, ci_char_traits<char>::compare("ABCD", "efgh", 4));

    EXPECT_LT(0, ci_char_traits<char>::compare("efgh", "abcd", 4));
    EXPECT_LT(0, ci_char_traits<char>::compare("efgh", "ABCD", 4));
    EXPECT_LT(0, ci_char_traits<char>::compare("EFGH", "abcd", 4));

    EXPECT_EQ(0, ci_char_traits<char>::compare("abc", "defg", 0));

    const char test[] = "AaBbCcDd";
    EXPECT_EQ(ci_char_traits<char>::find(test, 8, 'a') - test, 0);
    EXPECT_EQ(ci_char_traits<char>::find(test, 8, 'A') - test, 0);
    EXPECT_EQ(ci_char_traits<char>::find(test, 8, 'd') - test, 6);
    EXPECT_EQ(ci_char_traits<char>::find(test, 8, 'D') - test, 6);
    EXPECT_EQ(ci_char_traits<char>::find(test, 6, 'd'), nullptr);
    EXPECT_EQ(ci_char_traits<char>::find(test, 6, 'D'), nullptr);
}

TEST(Common, CiWCharTraits)
{
    using namespace mg;
    EXPECT_TRUE(ci_char_traits<wchar_t>::eq(L'a', L'a'));
    EXPECT_TRUE(ci_char_traits<wchar_t>::eq(L'A', L'a'));
    EXPECT_TRUE(ci_char_traits<wchar_t>::eq(L'a', L'A'));
    EXPECT_TRUE(ci_char_traits<wchar_t>::eq(L'ж', L'ж'));
    EXPECT_TRUE(ci_char_traits<wchar_t>::eq(L'Ж', L'ж'));
    EXPECT_TRUE(ci_char_traits<wchar_t>::eq(L'ж', L'Ж'));

    EXPECT_TRUE(ci_char_traits<wchar_t>::eq_int_type(L'a', L'a'));
    EXPECT_TRUE(ci_char_traits<wchar_t>::eq_int_type(L'A', L'a'));
    EXPECT_TRUE(ci_char_traits<wchar_t>::eq_int_type(L'a', L'A'));
    EXPECT_TRUE(ci_char_traits<wchar_t>::eq_int_type(L'ж', L'ж'));
    EXPECT_TRUE(ci_char_traits<wchar_t>::eq_int_type(L'Ж', L'ж'));
    EXPECT_TRUE(ci_char_traits<wchar_t>::eq_int_type(L'ж', L'Ж'));

    EXPECT_TRUE(ci_char_traits<wchar_t>::lt(L'a', L'b'));
    EXPECT_TRUE(ci_char_traits<wchar_t>::lt(L'A', L'b'));
    EXPECT_TRUE(ci_char_traits<wchar_t>::lt(L'a', L'B'));
    EXPECT_TRUE(ci_char_traits<wchar_t>::lt(L'ю', L'я'));
    EXPECT_TRUE(ci_char_traits<wchar_t>::lt(L'Ю', L'я'));
    EXPECT_TRUE(ci_char_traits<wchar_t>::lt(L'ю', L'Я'));

    EXPECT_EQ(0, ci_char_traits<wchar_t>::compare(L"test", L"test", 4));
    EXPECT_EQ(0, ci_char_traits<wchar_t>::compare(L"test", L"TEST", 4));
    EXPECT_EQ(0, ci_char_traits<wchar_t>::compare(L"TEST", L"test", 4));
    EXPECT_EQ(0, ci_char_traits<wchar_t>::compare(L"тест", L"тест", 4));
    EXPECT_EQ(0, ci_char_traits<wchar_t>::compare(L"тест", L"ТЕСТ", 4));
    EXPECT_EQ(0, ci_char_traits<wchar_t>::compare(L"ТЕСТ", L"тест", 4));

    EXPECT_EQ(0, ci_char_traits<wchar_t>::compare(L"testA", L"testD", 4));
    EXPECT_EQ(0, ci_char_traits<wchar_t>::compare(L"testB", L"TESTF", 4));
    EXPECT_EQ(0, ci_char_traits<wchar_t>::compare(L"TESTC", L"testG", 4));
    EXPECT_EQ(0, ci_char_traits<wchar_t>::compare(L"тестА", L"тестГ", 4));
    EXPECT_EQ(0, ci_char_traits<wchar_t>::compare(L"тестБ", L"ТЕСТД", 4));
    EXPECT_EQ(0, ci_char_traits<wchar_t>::compare(L"ТЕСТВ", L"тестЕ", 4));

    EXPECT_GT(0, ci_char_traits<wchar_t>::compare(L"abcd", L"efgh", 4));
    EXPECT_GT(0, ci_char_traits<wchar_t>::compare(L"abcd", L"EFGH", 4));
    EXPECT_GT(0, ci_char_traits<wchar_t>::compare(L"ABCD", L"efgh", 4));
    EXPECT_GT(0, ci_char_traits<wchar_t>::compare(L"абвг", L"деёж", 4));
    EXPECT_GT(0, ci_char_traits<wchar_t>::compare(L"абвг", L"ДЕЁЖ", 4));
    EXPECT_GT(0, ci_char_traits<wchar_t>::compare(L"АБВГ", L"деёж", 4));

    EXPECT_LT(0, ci_char_traits<wchar_t>::compare(L"efgh", L"abcd", 4));
    EXPECT_LT(0, ci_char_traits<wchar_t>::compare(L"efgh", L"ABCD", 4));
    EXPECT_LT(0, ci_char_traits<wchar_t>::compare(L"EFGH", L"abcd", 4));
    EXPECT_LT(0, ci_char_traits<wchar_t>::compare(L"деёж", L"абвг", 4));
    EXPECT_LT(0, ci_char_traits<wchar_t>::compare(L"деёж", L"АБВГ", 4));
    EXPECT_LT(0, ci_char_traits<wchar_t>::compare(L"ДЕЁЖ", L"абвг", 4));

    EXPECT_EQ(0, ci_char_traits<wchar_t>::compare(L"abc", L"defg", 0));

    const wchar_t test1[] = L"AaBbCcDd";
    EXPECT_EQ(ci_char_traits<wchar_t>::find(test1, 8, L'a') - test1, 0);
    EXPECT_EQ(ci_char_traits<wchar_t>::find(test1, 8, L'A') - test1, 0);
    EXPECT_EQ(ci_char_traits<wchar_t>::find(test1, 8, L'd') - test1, 6);
    EXPECT_EQ(ci_char_traits<wchar_t>::find(test1, 8, L'D') - test1, 6);
    EXPECT_EQ(ci_char_traits<wchar_t>::find(test1, 6, L'd'), nullptr);
    EXPECT_EQ(ci_char_traits<wchar_t>::find(test1, 6, L'D'), nullptr);

    const wchar_t test2[] = L"АаБбВвГг";
    EXPECT_EQ(ci_char_traits<wchar_t>::find(test2, 8, L'а') - test2, 0);
    EXPECT_EQ(ci_char_traits<wchar_t>::find(test2, 8, L'А') - test2, 0);
    EXPECT_EQ(ci_char_traits<wchar_t>::find(test2, 8, L'г') - test2, 6);
    EXPECT_EQ(ci_char_traits<wchar_t>::find(test2, 8, L'Г') - test2, 6);
    EXPECT_EQ(ci_char_traits<wchar_t>::find(test2, 6, L'г'), nullptr);
    EXPECT_EQ(ci_char_traits<wchar_t>::find(test2, 6, L'Г'), nullptr);
}


TEST_F(StandardAllocator, MoveAndCopyConstructWithOtherCharTraits)
{
    using namespace mg;

    stringref source("Test string");
    wstringref wsource(L"Test string");
    EXPECT_NE(source, "TEST STRING");
    EXPECT_NE(wsource, L"TEST STRING");

    cistringref s01(source);
    ciwstringref ws01(wsource);
    EXPECT_EQ(s01, "TEST STRING");
    EXPECT_EQ(ws01, L"TEST STRING");

    cistringref s02(source, 5, 6);
    ciwstringref ws02(wsource, 5, 6);
    EXPECT_EQ(s02, "STRING");
    EXPECT_EQ(ws02, L"STRING");

    cistringref s03(stringref("Test string"));
    ciwstringref ws03(wstringref(L"Test string"));
    EXPECT_EQ(s03, "TEST STRING");
    EXPECT_EQ(ws03, L"TEST STRING");

    cistringref s04(stringref("Test string"), 5, 6);
    ciwstringref ws04(wstringref(L"Test string"), 5, 6);
    EXPECT_EQ(s04, "STRING");
    EXPECT_EQ(ws04, L"STRING");

    cistringref s05(stringref("Test string"));
    ciwstringref ws05(wstringref(L"Test string"));
    EXPECT_EQ(s05, "TEST STRING");
    EXPECT_EQ(ws05, L"TEST STRING");

    cistringref s06(stringref(std::string("Test string")), 5, 6);
    ciwstringref ws06(wstringref(std::wstring(L"Test string")), 5, 6);
    EXPECT_EQ(s06, "STRING");
    EXPECT_EQ(ws06, L"STRING");
}

TEST_F(CustomAllocator, MoveAndCopyConstructWithOtherCharTraits)
{
    using namespace inplace;

    stringref source("Test string", a);
    wstringref wsource(L"Test string", a2);
    EXPECT_EQ(a.used_block_count(), static_cast<std::size_t>(0));
    EXPECT_EQ(a2.used_block_count(), static_cast<std::size_t>(0));
    EXPECT_NE(source, "TEST STRING");
    EXPECT_NE(wsource, L"TEST STRING");

    cistringref s01(source);
    ciwstringref ws01(wsource);
    EXPECT_EQ(a.used_block_count(), static_cast<std::size_t>(0));
    EXPECT_EQ(a2.used_block_count(), static_cast<std::size_t>(0));
    EXPECT_EQ(s01, "TEST STRING");
    EXPECT_EQ(ws01, L"TEST STRING");

    cistringref s02(source, 5, 6);
    ciwstringref ws02(wsource, 5, 6);
    EXPECT_EQ(a.used_block_count(), static_cast<std::size_t>(0));
    EXPECT_EQ(a2.used_block_count(), static_cast<std::size_t>(0));
    EXPECT_EQ(s02, "STRING");
    EXPECT_EQ(ws02, L"STRING");

    cistringref s03(stringref("Test string", a));
    ciwstringref ws03(wstringref(L"Test string", a2));
    EXPECT_EQ(a.used_block_count(), static_cast<std::size_t>(0));
    EXPECT_EQ(a2.used_block_count(), static_cast<std::size_t>(0));
    EXPECT_EQ(s03, "TEST STRING");
    EXPECT_EQ(ws03, L"TEST STRING");

    cistringref s04(stringref("Test string", a), 5, 6);
    ciwstringref ws04(wstringref(L"Test string", a2), 5, 6);
    EXPECT_EQ(a.used_block_count(), static_cast<std::size_t>(0));
    EXPECT_EQ(a2.used_block_count(), static_cast<std::size_t>(0));
    EXPECT_EQ(s04, "STRING");
    EXPECT_EQ(ws04, L"STRING");

    cistringref s05(stringref("Test string", a));
    ciwstringref ws05(wstringref(L"Test string", a2));
    EXPECT_EQ(a.used_block_count(), static_cast<std::size_t>(0));
    EXPECT_EQ(a2.used_block_count(), static_cast<std::size_t>(0));
    EXPECT_EQ(s05, "TEST STRING");
    EXPECT_EQ(ws05, L"TEST STRING");

    cistringref s06(stringref(std::string("Test string"), a), 5, 6);
    ciwstringref ws06(wstringref(std::wstring(L"Test string"), a2), 5, 6);
    EXPECT_EQ(a.used_block_count(), static_cast<std::size_t>(1));
    EXPECT_EQ(a2.used_block_count(), static_cast<std::size_t>(1));
    EXPECT_EQ(s06, "STRING");
    EXPECT_EQ(ws06, L"STRING");
}
