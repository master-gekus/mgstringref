#ifndef MGSTRINGREF_TEST_H
#define MGSTRINGREF_TEST_H

#include "mgstringref.h"

#include <cstring>
#include <new>
#include <string>
#include <memory>

#include <gtest/gtest.h>

namespace inplace {
    template <typename T>
    class allocator
    {
    public:
        typedef T value_type;
        typedef std::pair<std::size_t, std::size_t> counters_type;

        allocator() = delete;

        inline allocator(void* buffer, size_t buffer_size, size_t block_size = 4096) :
            buffer_(static_cast<char*>(buffer)),
            block_size_(block_size),
            counters_(new counters_type(0,0))
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
            block_count_(other.block_count_),
            counters_(other.counters_)
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
                    ++(counters_->first);
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
            ++(counters_->second);
            (*header) = 0;
        }

        bool operator ==(const allocator& other) const
        {
            return buffer_ == other.buffer_;
        }

        bool operator !=(const allocator& other) const
        {
            return buffer_ != other.buffer_;
        }

        size_t used_block_count() const
        {
            size_t count = 0;
            for (size_t i = 0; i < block_count_; i++) {
                if (0 != buffer_[i * block_size_]) {
                    count++;
                }
            }
            return count;
        }

        void clear_usage()
        {
            *counters_ = counters_type(0, 0);
        }

        size_t alloc_count() const
        {
            return counters_->first;
        }

        size_t dealloc_count() const
        {
            return counters_->second;
        }

    private:
        char *buffer_;
        size_t block_size_;
        size_t block_count_;
        std::shared_ptr<counters_type> counters_;

        template<typename U> friend class allocator;
    };

    typedef std::basic_string<char, std::char_traits<char>, allocator<char> > string;
    typedef std::basic_string<char16_t, std::char_traits<char16_t>, allocator<char16_t> > ustring;
    typedef std::basic_string<wchar_t, std::char_traits<wchar_t>, allocator<wchar_t> > wstring;
    typedef mg::basic_stringref<char, std::char_traits<char>, allocator<char> > stringref;
    typedef mg::basic_stringref<char16_t, std::char_traits<char16_t>, allocator<char16_t> > ustringref;
    typedef mg::basic_stringref<wchar_t, std::char_traits<wchar_t>, allocator<wchar_t> > wstringref;
    typedef mg::basic_stringref<char, mg::ci_char_traits<char>, allocator<char> > cistringref;
    typedef mg::basic_stringref<wchar_t, mg::ci_char_traits<wchar_t>, allocator<wchar_t> > ciwstringref;
}

static_assert(mg::stringref::allocator_is_always_equal, "Invalid std:allocator.");
static_assert(!inplace::stringref::allocator_is_always_equal, "Invalid inplace:allocator.");

class StandardAllocator : public ::testing::Test
{
protected:
    void CompareTest(const mg::stringref& s, const mg::wstringref& ws);
};

class CustomAllocator : public ::testing::Test
{
public:
    CustomAllocator();

protected:
    char buf[4096 * 100];
    char buf2[4096 * 100];
    ::inplace::allocator<char> a;
    ::inplace::allocator<char> a2;

    void TearDown() override;
    void CompareTest(const inplace::stringref& s, const inplace::wstringref& ws);
};

#endif // MGSTRINGREF_TEST_H
