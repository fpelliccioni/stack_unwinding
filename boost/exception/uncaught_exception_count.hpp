//             Copyright Evgeny Panasyuk 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// e-mail: E?????[dot]P???????[at]gmail.???

#ifndef BOOST_UNCAUGHT_EXCEPTION_COUNT_HPP_39A1E90FC11647e08D5F6ED16CD34B34
#define BOOST_UNCAUGHT_EXCEPTION_COUNT_HPP_39A1E90FC11647e08D5F6ED16CD34B34

#if defined(_MSC_VER) || defined(__GNUG__) || defined(__CLANG__)
    #define BOOST_UNCAUGHT_EXCEPTION_COUNT_SUPPORTED 1
#endif

namespace boost
{
    namespace exception_detail
    {
        template<typename To> inline
        To *unrelated_pointer_cast(void *from)
        {
            return static_cast<To*>(from);
        }
    }

    // uncaught_exception_count is a function similar to std::uncaught_exception from standard library,
    // but instead of boolean result it returns unsigned int showing current count of uncaught exceptions.

#if defined(_MSC_VER)
    namespace exception_detail
    {
        extern "C" char * __cdecl _getptd();
    }
    inline unsigned uncaught_exception_count()
    {
        // MSVC specific. Tested on {MSVC2005SP1,MSVC2008SP1,MSVC2010SP1,MSVC2012}x{x32,x64}.
        return *exception_detail::unrelated_pointer_cast<unsigned>
        (
            exception_detail::_getptd() + (sizeof(void*)==8 ? 0x100 : 0x90) 
        );
    }
#elif defined(__GNUG__) || defined(__CLANG__)
    namespace exception_detail
    {
        extern "C" char * __cxa_get_globals();
    }
    inline unsigned uncaught_exception_count()
    {
        // Tested on {Clang 3.2,GCC 3.4.6,GCC 4.1.2,GCC 4.4.6,GCC 4.4.7}x{x32,x64}
        return *exception_detail::unrelated_pointer_cast<unsigned>
        (
            exception_detail::__cxa_get_globals() + (sizeof(void*)==8 ? 0x8 : 0x4)
        );
    }
#endif

    // Within one scope uncaught_exception_count can be changed only by +1.
    // uncaught_exception_count_latch is primitive which helps to determine such transition.
    //     Internally it stores and compares only last bit of uncaught_exception_count value
#ifdef BOOST_UNCAUGHT_EXCEPTION_COUNT_SUPPORTED
    class uncaught_exception_count_latch
    {
            unsigned char enter_state;
        public:
            uncaught_exception_count_latch()
                : enter_state(static_cast<unsigned char>( uncaught_exception_count() & 1 ))
            {
            }
            bool transitioned() const
            {
                return enter_state != ( uncaught_exception_count() & 1 );
            }
    };
#endif

}

#endif
