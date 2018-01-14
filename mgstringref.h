#include <atomic>
#include <string>
#include <limits>
#include <cctype>
#include <cwctype>

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
        static constexpr const std::true_type detached{};

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

        void __int_construct_nc(const_pointer string, size_type size, size_type offset, size_type length, bool detach)
        {
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

        inline void __int_construct(const_pointer string, size_type size, size_type offset, size_type length,
                                    bool detach)
        {
            if ((nullptr == string) || (offset >= size) || (0 == length)) {
                return;
            }
            __int_construct_nc(string, size, offset, length, detach);
        }

        template<typename _OTraits>
        void __int_copy_construct(const basic_stringref<value_type, _OTraits, _Alloc>& other, size_type offset,
                                  size_type length)
        {
            if ((offset >= other.len_) || (0 == length)) {
                return;
            }
            if (other.d_) {
                if ((!allocator_is_always_equal) && (a_ != other.a_)) {
                    __int_construct(other.ptr_, other.len_, offset, length, true);
                    return;
                }
                d_ = reinterpret_cast<_Data*>(other.d_);
                ++(d_->ref_);
            }
            ptr_ = other.ptr_ + offset;
            len_ = std::min(length, other.len_ - offset);
        }

        template<typename _OTraits>
        void __int_move_construct(basic_stringref<value_type, _OTraits, _Alloc>& other, size_type offset,
                                  size_type length)
        {
            if ((offset < other.len_) && (0 != length)) {
                d_ = reinterpret_cast<_Data*>(other.d_);
                ptr_ = other.ptr_ + offset;
                len_ = std::min(length, other.len_ - offset);
            }
            other.d_ = nullptr;
            other.ptr_ = nullptr;
            other.len_ = 0;
        }

        void __int_release_data(_Data*& d)
        {
            if (d && (0 == (--d->ref_))) {
                d->~_Data();
                _Alloc_traits::deallocate(a_, reinterpret_cast<pointer>(d), d->allocated_ + _Data_Header_Len);
            }
            d = nullptr;
        }

        void __int_clear()
        {
            __int_release_data(d_);
            ptr_ = nullptr;
            len_ = 0;
        }

        basic_stringref& __int_assign(const_pointer string, size_type size, size_type offset, size_type length,
                                      bool detach)
        {
            // TODO: check for we can reuse allocated data.
            __int_clear();
            __int_construct(string, size, offset, length, detach);
            return *this;
        }

        template<typename _OTraits>
        basic_stringref& __int_copy_assign(const basic_stringref<value_type, _OTraits, _Alloc>& other,
                                           size_type offset, size_type length, bool copy_detach)
        {
            if ((offset >= other.len_) || (0 == length)) {
                __int_clear();
            } else if (copy_detach) {
                // TODO: check for we can reuse allocated data.
                __int_clear();
                __int_construct_nc(other.ptr_, other.len_, offset, length, true);
            } else if (!other.d_) {
                __int_clear();
                __int_construct_nc(other.ptr_, other.len_, offset, length, false);
            } else {
                if ((!allocator_is_always_equal) && (a_ != other.a_)) {
                    // TODO: check for we can reuse allocated data.
                    __int_clear();
                    __int_construct_nc(other.ptr_, other.len_, offset, length, true);
                } else {
                    __int_release_data(d_);
                    d_ = reinterpret_cast<_Data*>(other.d_);
                    ++(d_->ref_);
                    ptr_ = other.ptr_ + offset;
                    len_ = std::min(length, other.len_ - offset);
                }
            }
            return *this;
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

        basic_stringref(const_pointer string, std::true_type, const _Alloc& a = _Alloc()) :
            a_(a)
        {
            __int_construct(string, __int_strlen(string), 0, npos, true);
        }

        basic_stringref(const_pointer string, size_type size, const _Alloc& a = _Alloc()) :
            a_(a)
        {
            __int_construct(string, size, 0, size, false);
        }

        basic_stringref(const_pointer string, size_type size, std::true_type, const _Alloc& a = _Alloc()) :
            a_(a)
        {
            __int_construct(string, size, 0, size, true);
        }

        basic_stringref(const_pointer string, size_type offset, size_type length, const _Alloc& a = _Alloc()) :
            a_(a)
        {
            __int_construct(string, __int_strlen(string), offset, length, false);
        }

        basic_stringref(const_pointer string, size_type offset, size_type length, std::true_type, const _Alloc& a = _Alloc()) :
            a_(a)
        {
            __int_construct(string, __int_strlen(string), offset, length, true);
        }

        basic_stringref(const_pointer string, size_type size, size_type offset, size_type length,
                        const _Alloc& a = _Alloc()) :
            a_(a)
        {
            __int_construct(string, size, offset, length, false);
        }

        basic_stringref(const_pointer string, size_type size, size_type offset, size_type length,
                        std::true_type, const _Alloc& a = _Alloc()) :
            a_(a)
        {
            __int_construct(string, size, offset, length, true);
        }

        template<typename _OTraits, typename _OAlloc>
        explicit basic_stringref(const std::basic_string<value_type, _OTraits, _OAlloc>& string,
                                 const _Alloc& a = _Alloc()) :
            a_(a)
        {
            __int_construct(string.data(), string.size(), 0, npos, false);
        }

        template<typename _OTraits, typename _OAlloc>
        explicit basic_stringref(const std::basic_string<value_type, _OTraits, _OAlloc>& string,
                                 std::true_type, const _Alloc& a = _Alloc()) :
            a_(a)
        {
            __int_construct(string.data(), string.size(), 0, npos, true);
        }

        template<typename _OTraits, typename _OAlloc>
        basic_stringref(const std::basic_string<value_type, _OTraits, _OAlloc>& string, size_type offset,
                        size_type length, const _Alloc& a = _Alloc()) :
            a_(a)
        {
            __int_construct(string.data(), string.size(), offset, length, false);
        }

        template<typename _OTraits, typename _OAlloc>
        basic_stringref(const std::basic_string<value_type, _OTraits, _OAlloc>& string, size_type offset,
                        size_type length, std::true_type, const _Alloc& a = _Alloc()) :
            a_(a)
        {
            __int_construct(string.data(), string.size(), offset, length, true);
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
            a_(_Alloc_traits::select_on_container_copy_construction(other.a_))
        {
            __int_copy_construct(other, 0, other.len_);
        }

        basic_stringref(const basic_stringref& other, size_type offset, size_type length) :
            a_(_Alloc_traits::select_on_container_copy_construction(other.a_))
        {
            __int_copy_construct(other, offset, length);
        }

        template<typename _OTraits>
        explicit basic_stringref(const basic_stringref<value_type, _OTraits, _Alloc>& other) :
            a_(_Alloc_traits::select_on_container_copy_construction(other.a_))
        {
            __int_copy_construct(other, 0, other.len_);
        }

        template<typename _OTraits>
        basic_stringref(const basic_stringref<value_type, _OTraits, _Alloc>& other, size_type offset,
                        size_type length) :
            a_(_Alloc_traits::select_on_container_copy_construction(other.a_))
        {
            __int_copy_construct(other, offset, length);
        }

        template<typename _OTraits, typename _OAlloc>
        explicit basic_stringref(const basic_stringref<value_type, _OTraits, _OAlloc>& string,
                                 const _Alloc& a = _Alloc()) :
            a_(a)
        {
            __int_construct(string.data(), string.size(), 0, npos, string.is_detached());
        }

        template<typename _OTraits, typename _OAlloc>
        basic_stringref(const basic_stringref<value_type, _OTraits, _OAlloc>& string, size_type offset,
                        size_type length, const _Alloc& a = _Alloc()) :
            a_(a)
        {
            __int_construct(string.data(), string.size(), offset, length, string.is_detached());
        }

        basic_stringref(basic_stringref&& other) :
            a_(other.a_)
        {
            __int_move_construct(other, 0, other.len_);
        }

        basic_stringref(basic_stringref&& other, size_type offset, size_type length) :
            a_(other.a_)
        {
            __int_move_construct(other, offset, length);
        }

        template<typename _OTraits>
        explicit basic_stringref(basic_stringref<value_type, _OTraits, _Alloc>&& other) :
            a_(other.a_)
        {
            __int_move_construct(other, 0, other.len_);
        }

        template<typename _OTraits>
        basic_stringref(basic_stringref<value_type, _OTraits, _Alloc>&& other, size_type offset, size_type length) :
            a_(other.a_)
        {
            __int_move_construct(other, offset, length);
        }

        ~basic_stringref()
        {
            __int_release_data(d_);
        }

        inline basic_stringref& assign(const_pointer string)
        {
            return __int_assign(string, __int_strlen(string), 0, npos, false);
        }

        inline basic_stringref& assign(const_pointer string, std::true_type)
        {
            return __int_assign(string, __int_strlen(string), 0, npos, true);
        }

        inline basic_stringref& assign(const_pointer string, size_type size)
        {
            return __int_assign(string, size, 0, npos, false);
        }

        inline basic_stringref& assign(const_pointer string, size_type size, std::true_type)
        {
            return __int_assign(string, size, 0, npos, true);
        }

        inline basic_stringref& assign(const_pointer string, size_type offset, size_type length)
        {
            return __int_assign(string, __int_strlen(string), offset, length, false);
        }

        inline basic_stringref& assign(const_pointer string, size_type offset, size_type length, std::true_type)
        {
            return __int_assign(string, __int_strlen(string), offset, length, true);
        }

        inline basic_stringref& assign(const_pointer string, size_type size, size_type offset, size_type length)
        {
            return __int_assign(string, size, offset, length, false);
        }

        inline basic_stringref& assign(const_pointer string, size_type size, size_type offset, size_type length,
                                       std::true_type)
        {
            return __int_assign(string, size, offset, length, true);
        }

        template<typename _OTraits, typename _OAlloc>
        inline basic_stringref& assign(const std::basic_string<value_type, _OTraits, _OAlloc>& string)
        {
            return __int_assign(string.data(), string.size(), 0, npos, false);
        }

        template<typename _OTraits, typename _OAlloc>
        inline basic_stringref& assign(const std::basic_string<value_type, _OTraits, _OAlloc>& string, std::true_type)
        {
            return __int_assign(string.data(), string.size(), 0, npos, true);
        }

        template<typename _OTraits, typename _OAlloc>
        inline basic_stringref& assign(const std::basic_string<value_type, _OTraits, _OAlloc>& string,
                                       size_type offset, size_type length)
        {
            return __int_assign(string.data(), string.size(), offset, length, false);
        }

        template<typename _OTraits, typename _OAlloc>
        inline basic_stringref& assign(const std::basic_string<value_type, _OTraits, _OAlloc>& string,
                                       size_type offset, size_type length, std::true_type)
        {
            return __int_assign(string.data(), string.size(), offset, length, true);
        }

        template<typename _OTraits, typename _OAlloc>
        inline basic_stringref& assign(std::basic_string<value_type, _OTraits, _OAlloc>&& string)
        {
            return __int_assign(string.data(), string.size(), 0, npos, true);
        }

        template<typename _OTraits, typename _OAlloc>
        inline basic_stringref& assign(std::basic_string<value_type, _OTraits, _OAlloc>&& string, size_type offset,
                                size_type length)
        {
            return __int_assign(string.data(), string.size(), offset, length, true);
        }

        basic_stringref& assign(const basic_stringref& other)
        {
            return __int_copy_assign(other, 0, npos, false);
        }

        basic_stringref& assign(const basic_stringref& other, std::true_type)
        {
            return __int_copy_assign(other, 0, npos, true);
        }

        basic_stringref& assign(const basic_stringref& other, size_type offset, size_type length)
        {
            return __int_copy_assign(other, offset, length, false);
        }

        basic_stringref& assign(const basic_stringref& other, size_type offset, size_type length, std::true_type)
        {
            return __int_copy_assign(other, offset, length, true);
        }

        template<typename _OTraits, typename _OAlloc>
        inline basic_stringref& assign(const basic_stringref<value_type, _OTraits, _OAlloc>& other)
        {
            return __int_assign(other.data(), other.size(), 0, npos, other.is_detached());
        }

        template<typename _OTraits, typename _OAlloc>
        inline basic_stringref& assign(const basic_stringref<value_type, _OTraits, _OAlloc>& other, std::true_type)
        {
            return __int_assign(other.data(), other.size(), 0, npos, true);
        }

        template<typename _OTraits, typename _OAlloc>
        inline basic_stringref& assign(const basic_stringref<value_type, _OTraits, _OAlloc>& other, size_type offset,
                                       size_type length)
        {
            return __int_assign(other.data(), other.size(), offset, length, other.is_detached());
        }

        template<typename _OTraits, typename _OAlloc>
        inline basic_stringref& assign(const basic_stringref<value_type, _OTraits, _OAlloc>& other, size_type offset,
                                       size_type length, std::true_type)
        {
            return __int_assign(other.data(), other.size(), offset, length, true);
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

        basic_stringref& detach()
        {
            if (nullptr == d_) {
                __int_construct(ptr_, len_, 0, len_, true);
            }
            return *this;
        }

        bool is_detached() const
        {
            return (nullptr != d_);
        }

        inline int compare(const_pointer other) const
        {
            return __int_compare(ptr_, len_, other, __int_strlen(other));
        }

        inline int compare(const_pointer other, size_type other_size) const
        {
            return __int_compare(ptr_, len_, other, other_size);
        }

        template<typename _OTraits, typename _OAlloc>
        inline int compare(const std::basic_string<value_type, _OTraits, _OAlloc>& string) const
        {
            return __int_compare(ptr_, len_, string.data(), string.size());
        }

        inline int compare(const basic_stringref& other) const
        {
            if (this == &other) {
                return 0;
            }
            return __int_compare(ptr_, len_, other.ptr_, other.len_);
        }

        template<typename _OTraits, typename _OAlloc>
        inline int compare(const basic_stringref<value_type, _OTraits, _OAlloc>& other) const
        {
            return __int_compare(ptr_, len_, other.data(), other.size());
        }

        template<typename T>
        inline bool operator < (T other) const
        {
            return (0 > compare(other));
        }

        template<typename T>
        inline bool operator <= (T other) const
        {
            return (0 >= compare(other));
        }

        template<typename T>
        inline bool operator > (T other) const
        {
            return (0 < compare(other));
        }

        template<typename T>
        inline bool operator >= (T other) const
        {
            return (0 <= compare(other));
        }

        template<typename T>
        inline bool operator == (T other) const
        {
            return (0 == compare(other));
        }

        template<typename T>
        inline bool operator != (T other) const
        {
            return (0 != compare(other));
        }

    private:
        _Char_alloc_type a_;
        _Data *d_ = nullptr;
        const_pointer ptr_ = nullptr;
        size_type len_ = 0;

        template<typename C, typename T, typename A>
        friend class basic_stringref;
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

    template<typename _CharT>
    struct ci_char_traits;

    template<>
    struct ci_char_traits<char> : public std::char_traits<char>
    {
        static bool
        eq(const char_type& __c1, const char_type& __c2)
        {
            return std::toupper(static_cast<unsigned char>(__c1)) == std::toupper(static_cast<unsigned char>(__c2));
        }

        static bool
        eq_int_type(int_type __c1, int_type __c2)
        {
            return ::toupper(__c1) == ::toupper(__c2);
        }

        static bool
        lt(const char_type& __c1, const char_type& __c2)
        {
            return std::toupper(static_cast<unsigned char>(__c1)) < std::toupper(static_cast<unsigned char>(__c2));
        }

        static int
        compare(const char_type* __s1, const char_type* __s2, size_t __n)
        {
            while(__n--) {
                auto __c1 = std::toupper(static_cast<unsigned char>(*__s1));
                auto __c2 = std::toupper(static_cast<unsigned char>(*__s2));
                if (__c1 < __c2) {
                    return (-1);
                }
                if (__c1 > __c2) {
                    return 1;
                }
            }
            return 0;
        }

        static const char_type*
        find( const char_type* __p, std::size_t __n, const char_type& __c)
        {
            auto __cu = std::toupper(static_cast<unsigned char>(__c));
            for (; __n; --__n, ++__p) {
                if (std::toupper(static_cast<unsigned char>(*__p)) == __cu) {
                    return __p;
                }
            }
            return nullptr;
        }
    };

    template<>
    struct ci_char_traits<wchar_t> : public std::char_traits<wchar_t>
    {
        static bool
        eq(const char_type& __c1, const char_type& __c2)
        {
            return std::towupper(__c1) == std::towupper(__c2);
        }

        static bool
        eq_int_type(int_type __c1, int_type __c2)
        {
            return ::towupper(__c1) == ::towupper(__c2);
        }

        static bool
        lt(const char_type& __c1, const char_type& __c2)
        {
            return std::towupper(__c1) < std::towupper(__c2);
        }

        static int
        compare(const char_type* __s1, const char_type* __s2, size_t __n)
        {
            while(__n--) {
                auto __c1 = std::towupper(*__s1);
                auto __c2 = std::towupper(*__s2);
                if (__c1 < __c2) {
                    return (-1);
                }
                if (__c1 > __c2) {
                    return 1;
                }
            }
            return 0;
        }

        static const char_type*
        find( const char_type* __p, std::size_t __n, const char_type& __c)
        {
            auto __cu = std::towupper(__c);
            for (; __n; --__n, ++__p) {
                if (std::towupper(*__p) == __cu) {
                    return __p;
                }
            }
            return nullptr;
        }
    };

    typedef basic_stringref<char, ci_char_traits<char> > cistringref;
    typedef basic_stringref<wchar_t, ci_char_traits<wchar_t> > ciwstringref;
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

