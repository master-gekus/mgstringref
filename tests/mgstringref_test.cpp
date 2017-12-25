#include "mgstringref.h"

#include <cassert>
#include <new>
#include <string>
#include <map>

#include <gtest/gtest.h>

template <typename T>
class inplace_alloc
{
public:
    typedef T value_type;

    inplace_alloc() = delete;

    inline inplace_alloc(void* buffer, size_t buffer_size, size_t block_size = 4096) :
        buffer_(static_cast<char*>(buffer)),
        block_size_(block_size)
    {
        if ((nullptr == buffer_) || (0 == buffer_size) || (2 > block_size) || (block_size > buffer_size)) {
            throw std::bad_alloc();
        }
        block_count_ = buffer_size / block_size;
        memset(buffer_, 0, block_size_ * block_count_);
    }

    template <class U>
    inline inplace_alloc(const inplace_alloc<U>& other) noexcept :
        buffer_(other.buffer_),
        block_size_(other.block_size_),
        block_count_(other.block_count_)
    {
    }

    T* allocate(std::size_t n)
    {
        if ((n * sizeof(T)) > (block_size_ - 1)) {
            throw std::bad_alloc();
        }
        for (size_t i = 0; i < block_count_; i++) {
            if (0 == buffer_[i * block_size_]) {
                buffer_[i * block_size_] = 1;
                return reinterpret_cast<T*>(buffer_ + i * block_size_ + 1);
            }
        }
        throw std::bad_alloc();
    }

    void deallocate(T* p, std::size_t) noexcept
    {
        size_t offset = reinterpret_cast<char*>(p) - buffer_ - 1;
        if (offset > (block_count_ * block_size_) || (0 != (offset % block_size_))) {
            return; // Pointer not allocated by this allocator
        }
        buffer_[offset] = 0;
    }

private:
    char *buffer_;
    size_t block_size_;
    size_t block_count_;

    template<typename U> friend class inplace_alloc;
};

typedef std::basic_string<char, std::char_traits<char>, inplace_alloc<char> > inplace_string;

TEST(Helpers, CustomAllocator)
{
    char buf[4096 * 100];
    inplace_alloc<char> a(buf, sizeof(buf));
    inplace_string *s = new inplace_string("String must be lenght enough to allocated memory.", a);
    inplace_string s1(*s);
    inplace_string s2 = *s;
    delete s;
    EXPECT_EQ(s1, s2);

    std::map<inplace_string, inplace_string, std::less<inplace_string>,
            inplace_alloc<std::pair<inplace_string,inplace_string> > > m(a);
    m.emplace(inplace_string("String must be lenght enough to allocated memory.", a),
              inplace_string("String must be lenght enough to allocated memory.", a));

    EXPECT_EQ(m.size(), static_cast<size_t>(1));
    EXPECT_EQ(m.begin()->first, s1);
}
