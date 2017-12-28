#include "mgstringref.h"

#include <cassert>
#include <cstdio>
#include <new>
#include <string>
#include <map>

#include <gtest/gtest.h>

int main(int argc, char** argv)
{
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

namespace inplace {
    template <typename T>
    class allocator
    {
    public:
        typedef T value_type;

        allocator() = delete;

        inline allocator(void* buffer, size_t buffer_size, size_t block_size = 4096) :
            buffer_(static_cast<char*>(buffer)),
            block_size_(block_size)
        {
            if ((nullptr == buffer_) || (0 == buffer_size) || ((sizeof(std::size_t) + sizeof(T)) > block_size)
                || (block_size > buffer_size)) {
                throw std::bad_alloc();
            }
            block_count_ = buffer_size / block_size;
            memset(buffer_, 0, block_size_ * block_count_);
        }

        template <class U>
        inline allocator(const allocator<U>& other) noexcept :
            buffer_(other.buffer_),
            block_size_(other.block_size_),
            block_count_(other.block_count_)
        {
        }

        T* allocate(std::size_t n)
        {
            if ((n * sizeof(T)) > (block_size_ - sizeof(size_t))) {
                throw std::bad_alloc();
            }
            for (size_t i = 0; i < block_count_; i++) {
                std::size_t *header = reinterpret_cast<std::size_t*>(buffer_ + (i * block_size_));
                if (0 == (*header)) {
                    (*header) = (n * sizeof(T));
                    return reinterpret_cast<T*>(buffer_ + i * block_size_ + sizeof(size_t));
                }
            }
            throw std::bad_alloc();
        }

        void deallocate(T* p, std::size_t n)
        {
            size_t offset = reinterpret_cast<char*>(p) - buffer_ - sizeof(size_t);
            std::size_t *header = reinterpret_cast<std::size_t*>(buffer_ + offset);
            if (offset > (block_count_ * block_size_) || (0 != (offset % block_size_))
                || ((n * sizeof(T)) != (*header))) {
                throw std::bad_alloc();
            }
            (*header) = 0;
        }

        size_t used_block_count()
        {
            size_t count = 0;
            for (size_t i = 0; i < block_count_; i++) {
                if (0 != buffer_[i * block_size_]) {
                    count++;
                }
            }
            return count;
        }

    private:
        char *buffer_;
        size_t block_size_;
        size_t block_count_;

        template<typename U> friend class allocator;
    };

    typedef std::basic_string<char, std::char_traits<char>, allocator<char> > string;
    typedef std::basic_string<char16_t, std::char_traits<char16_t>, allocator<char16_t> > ustring;
    typedef std::basic_string<wchar_t, std::char_traits<wchar_t>, allocator<wchar_t> > wstring;
    typedef mg::basic_stringref<char, std::char_traits<char>, allocator<char> > stringref;
    typedef mg::basic_stringref<char16_t, std::char_traits<char16_t>, allocator<char16_t> > ustringref;
    typedef mg::basic_stringref<wchar_t, std::char_traits<wchar_t>, allocator<wchar_t> > wstringref;
}

class StandardAllocator : public ::testing::Test
{
protected:
    void CompareTest(const mg::stringref& s, const mg::wstringref& ws);
};

class CustomAllocator : public ::testing::Test
{
public:
    CustomAllocator() :
        a(buf, sizeof(buf))
    {}

protected:
    char buf[4096 * 100];
    ::inplace::allocator<char> a;

    void TearDown() override
    {
        EXPECT_EQ(a.used_block_count(), static_cast<std::size_t>(0));
    }

    void CompareTest(const inplace::stringref& s, const inplace::wstringref& ws);
};

TEST_F(CustomAllocator, StandardContainers)
{
    using namespace inplace;
    string *s = new string("String must be lenght enough to allocated memory.", a);
    string s1(*s);
    string s2 = *s;
    EXPECT_EQ(a.used_block_count(), static_cast<std::size_t>(3));
    delete s;
    EXPECT_EQ(s1, s2);
    EXPECT_EQ(a.used_block_count(), static_cast<std::size_t>(2));

    std::map<string, string, std::less<string>, allocator<std::map<string, string>::value_type> > m(a);
    m.emplace(string("String must be lenght enough to allocated memory.", a),
              string("String must be lenght enough to allocated memory.", a));

    EXPECT_EQ(m.size(), static_cast<std::size_t>(1));
    EXPECT_EQ(m.begin()->first, s1);
}

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
    EXPECT_EQ(a.used_block_count(), static_cast<std::size_t>(2));
    EXPECT_EQ(scopy_copy2, "c");
    EXPECT_EQ(wscopy_copy2, L"c");

    mg::stringref scopy_copy3(scopy, 1, 1);
    mg::wstringref wscopy_copy3(wscopy, 1, 1);
    EXPECT_EQ(a.used_block_count(), static_cast<std::size_t>(2));
    EXPECT_EQ(scopy_copy3, "c");
    EXPECT_EQ(wscopy_copy3, L"c");
}
