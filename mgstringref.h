#include <atomic>
#include <string>

namespace mg {
    template<typename _CharT, typename _Traits = std::char_traits<_CharT>,
             typename _Alloc = std::allocator<_CharT> >
    class basic_stringref final
    {
        typedef typename std::allocator_traits<_Alloc>::template
            rebind_alloc<_CharT> _Char_alloc_type;
        typedef std::allocator_traits<_Char_alloc_type> _Alloc_traits;

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

    private:
        struct _Data {
            constexpr _Data(const_pointer ptr, size_type len, size_type allocated, int ref) :
                ref_(ref), len_(len), allocated_(allocated), ptr_(ptr)
            {}

            mutable std::atomic<int> ref_;
            size_type len_;
            size_type allocated_;
            const_pointer ptr_;
        };
        static_assert(0 == (sizeof(_Data) % sizeof(value_type)), "Invalid aligment.");
        static constexpr const std::size_t _Data_Header_Len = sizeof(_Data) / sizeof(value_type);

        size_type __int_strlen(const_pointer string)
        {
            return (nullptr == string) ? 0 : _Traits::length(string);
        }

        void __int_construct_ref(const_pointer string, size_type size)
        {
            if ((nullptr == string) || (0 == size)) {
                return;
            }
            pointer data = _Alloc_traits::allocate(a_, _Data_Header_Len);
            d_ = new(data) _Data(string, size, 0, 1);
        }

        void __int_construct_copy(const_pointer string, size_type size)
        {
            if ((nullptr == string) || (0 == size)) {
                return;
            }
            pointer data = _Alloc_traits::allocate(a_, _Data_Header_Len + size);
            d_ = new(data) _Data(data + _Data_Header_Len, size, size, 1);
            _Traits::copy(data + _Data_Header_Len, string, size);
        }

        void __int_construct_with_offset(const_pointer string, size_type size, size_type offset, size_type length,
                                        void (basic_stringref::*constructor)(const_pointer string, size_type size))
        {
            if (offset >= size) {
                return;
            }
            (this->*constructor)(string, size);
            offset_ = offset;
            len_ = ((offset + length) > size) ? (size - offset) : length;
        }

        void __int_release_data()
        {
            if (d_ && (0 == (--d_->ref_))) {
                d_->~_Data();
                _Alloc_traits::deallocate(a_, reinterpret_cast<pointer>(d_), d_->allocated_ + _Data_Header_Len);
            }
            d_ = nullptr;
        }

    public:
        explicit basic_stringref(const _Alloc& a = _Alloc()) :
            d_(nullptr), offset_(0), len_(0), a_(a)
        {}

        explicit basic_stringref(const_pointer string, const _Alloc& a = _Alloc()) :
            d_(nullptr), offset_(0), len_(__int_strlen(string)), a_(a)
        {
            __int_construct_ref(string, len_);
        }

        basic_stringref(const_pointer string, size_type size, const _Alloc& a = _Alloc()) :
            d_(nullptr), offset_(0), len_(size), a_(a)
        {
            __int_construct_ref(string, size);
        }

        basic_stringref(const_pointer string, size_type offset, size_type length, const _Alloc& a = _Alloc()) :
            d_(nullptr), offset_(0), len_(0), a_(a)
        {
            __int_construct_with_offset(string, __int_strlen(string), offset, length,
                                        &basic_stringref::__int_construct_ref);
        }

        basic_stringref(const_pointer string, size_type size, size_type offset, size_type length,
                        const _Alloc& a = _Alloc()) :
            d_(nullptr), offset_(0), len_(0), a_(a)
        {
            __int_construct_with_offset(string, size, offset, length, &basic_stringref::__int_construct_ref);
        }

        template<typename _OTraits, typename _OAlloc>
        basic_stringref(const std::basic_string<value_type, _OTraits, _OAlloc>& string, const _Alloc& a = _Alloc()) :
            d_(nullptr), offset_(0), len_(string.size()), a_(a)
        {
            __int_construct_ref(string.data(), len_);
        }

        template<typename _OTraits, typename _OAlloc>
        basic_stringref(const std::basic_string<value_type, _OTraits, _OAlloc>& string, size_type offset,
                        size_type length, const _Alloc& a = _Alloc()) :
            d_(nullptr), offset_(0), len_(0), a_(a)
        {
            __int_construct_with_offset(string.data(), string.size(), offset, length,
                                        &basic_stringref::__int_construct_ref);
        }

        template<typename _OTraits, typename _OAlloc>
        explicit basic_stringref(std::basic_string<value_type, _OTraits, _OAlloc>&& string,
                                 const _Alloc& a = _Alloc()) :
            d_(nullptr), offset_(0), len_(string.size()), a_(a)
        {
            __int_construct_copy(string.data(), len_);
        }

        template<typename _OTraits, typename _OAlloc>
        basic_stringref(std::basic_string<value_type, _OTraits, _OAlloc>&& string, size_type offset, size_type length,
                        const _Alloc& a = _Alloc()) :
            d_(nullptr), offset_(0), len_(0), a_(a)
        {
            __int_construct_with_offset(string.data(), string.size(), offset, length,
                                        &basic_stringref::__int_construct_copy);
        }

        ~basic_stringref()
        {
            __int_release_data();
        }

        bool empty() const
        {
            return (0 == len_);
        }

        size_type size() const
        {
            return len_;
        }

    private:
        _Data *d_;
        size_type offset_;
        size_type len_;
        _Char_alloc_type a_;
    };

    typedef basic_stringref<char> stringref;
    typedef basic_stringref<char16_t> ustringref;
    typedef basic_stringref<wchar_t> wstringref;
}
