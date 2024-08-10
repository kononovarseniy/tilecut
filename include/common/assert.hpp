#pragma once

#include <common/log.hpp>

#ifdef NDEBUG
    #define AR_ASSERT(condition) ((void)0)
    #define AR_PRE(condition) ((void)0)
    #define AR_POST(condition) ((void)0)
    #define AR_NESTED_ASSERT(condition, assert_type, location)                                                         \
        ((void)(condition), (void)(assert_type), (void)(location))
    #define AR_UNREACHABLE ::r7::__assert_detail::unreachable()
#else
    #define AR_NESTED_ASSERT(condition, assert_type, location)                                                         \
        do                                                                                                             \
        {                                                                                                              \
            if (!(condition))                                                                                          \
            {                                                                                                          \
                ::r7::__assert_detail::assert_handler(assert_type, #condition, location);                              \
            }                                                                                                          \
        } while (false)
    #define AR_ASSERT(condition) AR_NESTED_ASSERT(condition, "Assertion", std::source_location::current())
    #define AR_PRE(condition) AR_NESTED_ASSERT(condition, "Precondition", std::source_location::current())
    #define AR_POST(condition) AR_NESTED_ASSERT(condition, "Postcondition", std::source_location::current())
    #define AR_UNREACHABLE                                                                                             \
        do                                                                                                             \
        {                                                                                                              \
            ::r7::__assert_detail::assert_handler("Unreachable", "unreachable", std::source_location::current());      \
            ::r7::__assert_detail::unreachable();                                                                      \
        } while (false)
#endif

namespace r7::__assert_detail
{

inline void assert_handler(
    const std::string_view assert_type,
    const std::string_view condition,
    const std::source_location & location)
{
    log_assert(assert_type, condition, location);
    std::abort();
}

// TODO: remove in C++23
[[noreturn]] inline void unreachable()
{
    // From https://en.cppreference.com/w/cpp/utility/unreachable
    // Uses compiler specific extensions if possible.
    // Even if no extension is used, undefined behavior is still raised by
    // an empty function body and the noreturn attribute.
#if defined(__GNUC__) // GCC, Clang, ICC
    __builtin_unreachable();
#elif defined(_MSC_VER) // MSVC
    __assume(false);
#endif
}

} // namespace r7::__assert_detail
