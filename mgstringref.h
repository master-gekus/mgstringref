#include <atomic>
#include <string>

namespace mg {
    template<typename _CharT, typename _Traits = std::char_traits<_CharT>,
             typename _Alloc = std::allocator<_CharT> >
    class basic_stringref
    {
        typedef typename std::allocator_traits<_Alloc>::template
            rebind<_CharT>::other _Char_alloc_type;
        typedef std::allocator_traits<_Char_alloc_type> _Alloc_traits;

    public:
        typedef _Traits                                 traits_type;
        typedef typename _Traits::char_type             value_type;
        typedef _Char_alloc_type                        allocator_type;
        typedef typename _Alloc_traits::size_type       size_type;
        typedef typename _Alloc_traits::difference_type difference_type;
        typedef typename _Alloc_traits::reference       reference;
        typedef typename _Alloc_traits::const_reference const_reference;
        typedef typename _Alloc_traits::pointer         pointer;
        typedef typename _Alloc_traits::const_pointer   const_pointer;
    };

    typedef basic_stringref<char>string;
}



