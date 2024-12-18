#pragma once

#include <ka/common/log.hpp>

#if defined(__GNUC__) // GCC, Clang, ICC
    #define _KA_UNREACHABLE __builtin_unreachable()
#elif defined(_MSC_VER) // MSVC
    #define _KA_UNREACHABLE __assume(false)
#endif

#ifdef NDEBUG
    #define AR_ASSERT(condition) ((void)0)
    #define AR_PRE(condition) ((void)0)
    #define AR_POST(condition) ((void)0)
    #define AR_NESTED_ASSERT(condition, assert_type, location)                                                         \
        ((void)(condition), (void)(assert_type), (void)(location))
    #define AR_UNREACHABLE _KA_UNREACHABLE
#else
    #define AR_NESTED_ASSERT(condition, assert_type, location)                                                         \
        do                                                                                                             \
        {                                                                                                              \
            if (!(condition))                                                                                          \
            {                                                                                                          \
                ::ka::__assert_detail::assert_handler(assert_type, #condition, location);                              \
            }                                                                                                          \
        } while (false)
    #define AR_ASSERT(condition) AR_NESTED_ASSERT(condition, "Assertion", std::source_location::current())
    #define AR_PRE(condition) AR_NESTED_ASSERT(condition, "Precondition", std::source_location::current())
    #define AR_POST(condition) AR_NESTED_ASSERT(condition, "Postcondition", std::source_location::current())
    #define AR_UNREACHABLE                                                                                             \
        do                                                                                                             \
        {                                                                                                              \
            ::ka::__assert_detail::assert_handler("Unreachable", "unreachable", std::source_location::current());      \
            _KA_UNREACHABLE;                                                                                           \
        } while (false)
#endif

namespace ka::__assert_detail
{

inline void assert_handler(
    const std::string_view assert_type,
    const std::string_view condition,
    const std::source_location & location)
{
    log_assert(assert_type, condition, location);
    std::abort();
}

} // namespace ka::__assert_detail
