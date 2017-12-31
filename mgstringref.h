#include <atomic>
#include <string>
#include <limits>

namespace mg {
    template<typename _CharT, typename _Traits = std::char_traits<_CharT>,
             typename _Alloc = std::allocator<_CharT> >
    class basic_stringref final
    {
        typedef typename std::allocator_traits<_Alloc>::template
            rebind_alloc<_CharT> _Char_alloc_type;
        typedef std::allocator_traits<_Char_alloc_type> _Alloc_traits;

        template<class T>
        struct __int_void
        { typedef void type; };

        template<class T, class U = void>
        struct __int_is_always_equal
        { static constexpr const bool value = std::is_empty<T>::value; };

        template<class T>
        struct __int_is_always_equal<T,
                typename __int_void<typename std::allocator_traits<typename T::is_always_equal> >::type>
        { static constexpr const bool value = T::is_always_equal::value; };

    public:
        typedef _Traits                                 traits_type;
        typedef typename _Traits::char_type             value_type;
        typedef _Char_alloc_type                        allocator_type;
        typedef typename _Alloc_traits::size_type       size_type;
        typedef typename _Alloc_traits::difference_type difference_type;
        typedef value_type&                             reference;
        typedef value_type const&                       const_reference;
        typedef typename _Alloc_traits::pointer         pointer;
        typedef typename _Alloc_traits::const_pointer   const_pointer;

        static constexpr const size_type npos = std::numeric_limits<size_type>::max();
        static constexpr const bool allocator_is_always_equal = __int_is_always_equal<_Alloc>::value;

    private:
        struct _Data {
            constexpr _Data(int ref, size_type allocated) :
                ref_(ref), allocated_(allocated)
            {}

            mutable std::atomic<int> ref_;
            size_type allocated_;
        };
        static_assert(0 == (sizeof(_Data) % sizeof(value_type)), "Invalid aligment.");
        static constexpr const std::size_t _Data_Header_Len = sizeof(_Data) / sizeof(value_type);

        void __int_construct(const_pointer string, size_type size, size_type offset, size_type length, bool detach)
        {
            if ((nullptr == string) || (offset >= size) || (0 == length)) {
                return;
            }

            if (0 != offset) {
                string += offset;
                size -= offset;
            }

            if (detach) {
                pointer data = _Alloc_traits::allocate(a_, _Data_Header_Len + size);
                d_ = new(data) _Data(1, size);
                ptr_ = data + _Data_Header_Len;
                _Traits::copy(data + _Data_Header_Len, string, size);
            } else {
                ptr_ = string;
            }

            // If lenght == npos, we will use size - offset, because npos is maximum value of size_type
            len_ = std::min(size, length);
        }

        void __int_release_data(_Data*& d)
        {
            if (d && (0 == (--d->ref_))) {
                d->~_Data();
                _Alloc_traits::deallocate(a_, reinterpret_cast<pointer>(d), d->allocated_ + _Data_Header_Len);
            }
            d = nullptr;
        }

        static size_type __int_strlen(const_pointer string)
        {
            return (nullptr == string) ? 0 : _Traits::length(string);
        }

        static int __int_compare(const_pointer s1, size_type size1, const_pointer s2, size_t size2)
        {
            int result = _Traits::compare(s1, s2, std::min(size1, size2));
            if (size1 == size2) {
                return result;
            } else {
                return result ? result : (size1 < size2) ? (-1) : 1;
            }
        }

    public:
        explicit basic_stringref(const _Alloc& a = _Alloc()) :
            a_(a)
        {}

        explicit basic_stringref(const_pointer string, const _Alloc& a = _Alloc()) :
            a_(a)
        {
            __int_construct(string, __int_strlen(string), 0, npos, false);
        }

        basic_stringref(const_pointer string, size_type size, const _Alloc& a = _Alloc()) :
            a_(a)
        {
            __int_construct(string, size, 0, size, false);
        }

        basic_stringref(const_pointer string, size_type offset, size_type length, const _Alloc& a = _Alloc()) :
            a_(a)
        {
            __int_construct(string, __int_strlen(string), offset, length, false);
        }

        basic_stringref(const_pointer string, size_type size, size_type offset, size_type length,
                        const _Alloc& a = _Alloc()) :
            a_(a)
        {
            __int_construct(string, size, offset, length, false);
        }

        template<typename _OTraits, typename _OAlloc>
        explicit basic_stringref(const std::basic_string<value_type, _OTraits, _OAlloc>& string,
                                 const _Alloc& a = _Alloc()) :
            a_(a)
        {
            __int_construct(string.data(), string.size(), 0, npos, false);
        }

        template<typename _OTraits, typename _OAlloc>
        basic_stringref(const std::basic_string<value_type, _OTraits, _OAlloc>& string, size_type offset,
                        size_type length, const _Alloc& a = _Alloc()) :
            a_(a)
        {
            __int_construct(string.data(), string.size(), offset, length, false);
        }

        template<typename _OTraits, typename _OAlloc>
        explicit basic_stringref(std::basic_string<value_type, _OTraits, _OAlloc>&& string,
                                 const _Alloc& a = _Alloc()) :
            a_(a)
        {
            __int_construct(string.data(), string.size(), 0, npos, true);
        }

        template<typename _OTraits, typename _OAlloc>
        basic_stringref(std::basic_string<value_type, _OTraits, _OAlloc>&& string, size_type offset, size_type length,
                        const _Alloc& a = _Alloc()) :
            a_(a)
        {
            __int_construct(string.data(), string.size(), offset, length, true);
        }

        basic_stringref(const basic_stringref& other) :
            a_(other.a_), d_(other.d_), ptr_(other.ptr_), len_(other.len_)
        {
            if (d_) {
                ++(d_->ref_);
            }
        }

        basic_stringref(const basic_stringref& other, size_type offset, size_type length) :
            a_(other.a_)
        {
            if ((offset >= other.len_) || (0 == length)) {
                return;
            }
            if (other.d_) {
                d_ = other.d_;
                ++(d_->ref_);
            }
            ptr_ = other.ptr_ + offset;
            len_ = std::min(length, other.len_ - offset);
        }

        template<typename _OTraits, typename _OAlloc>
        explicit basic_stringref(const basic_stringref<value_type, _OTraits, _OAlloc>& string,
                                 const _Alloc& a = _Alloc()) :
            a_(a)
        {
            __int_construct(string.data(), string.size(), 0, npos, false);
        }

        template<typename _OTraits, typename _OAlloc>
        basic_stringref(const basic_stringref<value_type, _OTraits, _OAlloc>& string, size_type offset,
                        size_type length, const _Alloc& a = _Alloc()) :
            a_(a)
        {
            __int_construct(string.data(), string.size(), offset, length, false);
        }

        basic_stringref(basic_stringref&& other) :
            a_(other.a_), d_(other.d_), ptr_(other.ptr_), len_(other.len_)
        {
            other.d_ = nullptr;
            other.ptr_ = nullptr;
            other.len_ = 0;
        }

        basic_stringref(basic_stringref&& other, size_type offset, size_type length) :
            a_(other.a_)
        {
            if ((offset < other.len_) && (0 != length)) {
                d_ = other.d_;
                ptr_ = other.ptr_ + offset;
                len_ = std::min(length, other.len_ - offset);
            }
            other.d_ = nullptr;
            other.ptr_ = nullptr;
            other.len_ = 0;
        }

        ~basic_stringref()
        {
            __int_release_data(d_);
        }

        bool empty() const
        {
            return (0 == len_);
        }

        size_type size() const
        {
            return len_;
        }

        const_pointer data() const
        {
            return ptr_;
        }

        int compare(const_pointer other) const
        {
            return __int_compare(ptr_, len_, other, __int_strlen(other));
        }

        int compare(const_pointer other, size_type other_size) const
        {
            return __int_compare(ptr_, len_, other, other_size);
        }

        template<typename _OTraits, typename _OAlloc>
        int compare(const std::basic_string<value_type, _OTraits, _OAlloc>& string) const
        {
            return __int_compare(ptr_, len_, string.data(), string.size());
        }

        int compare(const basic_stringref& other) const
        {
            if (this == &other) {
                return 0;
            }
            return __int_compare(ptr_, len_, other.ptr_, other.len_);
        }

        template<typename _OTraits, typename _OAlloc>
        int compare(const basic_stringref<value_type, _OTraits, _OAlloc>& other) const
        {
            return __int_compare(ptr_, len_, other.data(), other.size());
        }

        template<typename T>
        bool operator < (T other) const
        {
            return (0 > compare(other));
        }

        template<typename T>
        bool operator <= (T other) const
        {
            return (0 >= compare(other));
        }

        template<typename T>
        bool operator > (T other) const
        {
            return (0 < compare(other));
        }

        template<typename T>
        bool operator >= (T other) const
        {
            return (0 <= compare(other));
        }

        template<typename T>
        bool operator == (T other) const
        {
            return (0 == compare(other));
        }

        template<typename T>
        bool operator != (T other) const
        {
            return (0 != compare(other));
        }

    private:
        _Char_alloc_type a_;
        _Data *d_ = nullptr;
        const_pointer ptr_ = nullptr;
        size_type len_ = 0;
    };

    template <typename T>
    struct is_stringref
    {
        static constexpr bool value = false;
    };

    template<typename _CharT, typename _Traits, typename _Alloc>
    struct is_stringref<basic_stringref<_CharT, _Traits, _Alloc> >
    {
        static constexpr bool value = true;
    };

    typedef basic_stringref<char> stringref;
    typedef basic_stringref<char16_t> ustringref;
    typedef basic_stringref<wchar_t> wstringref;
}

template<typename T, typename _CharT, typename _Traits, typename _Alloc>
typename std::enable_if<!mg::is_stringref<T>::value, bool>::type
inline operator < (T s1, const mg::basic_stringref<_CharT, _Traits, _Alloc>& s2)
{
    return (0 < s2.compare(s1));
}

template<typename T, typename _CharT, typename _Traits, typename _Alloc>
typename std::enable_if<!mg::is_stringref<T>::value, bool>::type
inline operator <= (T s1, const mg::basic_stringref<_CharT, _Traits, _Alloc>& s2)
{
    return (0 <= s2.compare(s1));
}

template<typename T, typename _CharT, typename _Traits, typename _Alloc>
typename std::enable_if<!mg::is_stringref<T>::value, bool>::type
inline operator > (T s1, const mg::basic_stringref<_CharT, _Traits, _Alloc>& s2)
{
    return (0 > s2.compare(s1));
}

template<typename T, typename _CharT, typename _Traits, typename _Alloc>
typename std::enable_if<!mg::is_stringref<T>::value, bool>::type
inline operator >= (T s1, const mg::basic_stringref<_CharT, _Traits, _Alloc>& s2)
{
    return (0 >= s2.compare(s1));
}

template<typename T, typename _CharT, typename _Traits, typename _Alloc>
typename std::enable_if<!mg::is_stringref<T>::value, bool>::type
inline operator == (T s1, const mg::basic_stringref<_CharT, _Traits, _Alloc>& s2)
{
    return (0 == s2.compare(s1));
}

template<typename T, typename _CharT, typename _Traits, typename _Alloc>
typename std::enable_if<!mg::is_stringref<T>::value, bool>::type
inline operator != (T s1, const mg::basic_stringref<_CharT, _Traits, _Alloc>& s2)
{
    return (0 != s2.compare(s1));
}

