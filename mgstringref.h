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
            mutable std::atomic<int> ref_;
            difference_type len_;
            difference_type allocated_;
            value_type const* ptr_;
#ifdef _MSC_VER
#pragma warning(disable: 4200)
#endif
            alignas(alignof(value_type)) value_type data_[];
#ifdef _MSC_VER
#pragma warning(default: 4200)
#endif
        };
        static_assert(0 == (sizeof(_Data) % sizeof(value_type)), "Invalid aligment.");
        static constexpr const std::size_t _Data_Header_Len = sizeof(_Data) / sizeof(value_type);

    public:
        explicit basic_stringref(const _Alloc& a = _Alloc()) :
            d_(nullptr), offset_(0), len_(0), a_(a)
        {}

        ~basic_stringref()
        {
            if (d_ && (0 == (--d_->ref_))) {
                _Alloc_traits::deallocate(a_, reinterpret_cast<value_type*>(d_), d_->allocated_ + _Data_Header_Len);
            }
        }

        bool empty() const
        {
            return (0 == len_);
        }

    private:
        _Data *d_;
        difference_type offset_;
        difference_type len_;
        _Char_alloc_type a_;
    };

    typedef basic_stringref<char> stringref;
    typedef basic_stringref<char16_t> ustringref;
    typedef basic_stringref<wchar_t> wstringref;
}



