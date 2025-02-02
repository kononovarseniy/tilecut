#pragma once

#include <cmath>
#include <concepts>
#include <numeric>
#include <source_location>
#include <type_traits>
#include <utility>

#include <ka/common/assert.hpp>
#include <ka/common/fixed.hpp>

namespace ka
{

template <std::integral I, ieee_float F>
[[nodiscard]] consteval F max_common_representable_number() noexcept
{
    constexpr auto extra_int_digits = std::numeric_limits<I>::digits - std::numeric_limits<F>::digits;
    static_assert(extra_int_digits > 0, "This is not the function you are looking for");

    constexpr I int_one = 1;
    constexpr I result = std::numeric_limits<I>::max() - ((int_one << extra_int_digits) - int_one);

    static_assert(static_cast<I>(static_cast<F>(result) == result));

    return static_cast<F>(result);
}

template <std::signed_integral I, ieee_float F>
[[nodiscard]] consteval F min_common_representable_number() noexcept
{
    constexpr auto result = std::numeric_limits<I>::min();
    return static_cast<F>(result);
}

template <std::unsigned_integral Target, ieee_float Source>
[[nodiscard]] constexpr bool in_exact_range(const Source value)
{
    return Target {} <= value && value <= max_common_representable_number<Target, Source>();
}

template <std::signed_integral Target, ieee_float Source>
[[nodiscard]] constexpr bool in_exact_range(const Source value)
{
    return value >= min_common_representable_number<Target, Source>() &&
           value <= max_common_representable_number<Target, Source>();
}

template <std::integral Target, std::integral Source>
[[nodiscard]] constexpr bool in_exact_range(const Source value)
{
    return std::in_range<Target>(value);
}

template <std::integral T, ieee_float S>
[[nodiscard]] constexpr T exact_cast(
    const S & value,
    const std::source_location & location = std::source_location::current()) noexcept
{
    constexpr auto assert_type = "exact_cast float -> int";
    AR_NESTED_ASSERT(in_exact_range<T>(value), assert_type, location);
    AR_NESTED_ASSERT(static_cast<S>(static_cast<T>(value)) == value, assert_type, location);
    return static_cast<T>(value);
}

template <ieee_float T, std::integral S>
[[nodiscard]] constexpr T exact_cast(
    const S & value,
    const std::source_location & location = std::source_location::current()) noexcept
{
    constexpr auto assert_type = "exact_cast int -> float";

    static_assert(std::numeric_limits<S>::max() <= std::numeric_limits<T>::max());
    static_assert(std::numeric_limits<S>::min() >= std::numeric_limits<T>::lowest());

    AR_NESTED_ASSERT(static_cast<S>(static_cast<T>(value)) == value, assert_type, location);
    return static_cast<T>(value);
}

template <ieee_float T, ieee_float S>
[[nodiscard]] constexpr T exact_cast(
    const S & value,
    const std::source_location & location = std::source_location::current()) noexcept
{
    constexpr auto assert_type = "exact_cast float -> float";
    AR_NESTED_ASSERT(value >= std::numeric_limits<T>::lowest(), assert_type, location);
    AR_NESTED_ASSERT(value <= std::numeric_limits<T>::max(), assert_type, location);
    AR_NESTED_ASSERT(static_cast<S>(static_cast<T>(value)) == value, assert_type, location);
    return static_cast<T>(value);
}

template <std::integral T, std::integral S>
[[nodiscard]] constexpr T exact_cast(
    const S & value,
    const std::source_location & location = std::source_location::current()) noexcept
{
    AR_NESTED_ASSERT(in_exact_range<T>(value), "exact_cast int -> int", location);
    return static_cast<T>(value);
}

template <std::integral T, std::integral S>
[[nodiscard]] constexpr T safe_cast(const S & value) noexcept
    requires(
        (std::numeric_limits<S>::max() <= std::numeric_limits<T>::max()) &&
        (std::numeric_limits<S>::min() >= std::numeric_limits<T>::min()))
{
    return static_cast<T>(value);
}

template <ieee_float T, ieee_float S>
[[nodiscard]] constexpr T safe_cast(const S & value) noexcept
    requires(
        (std::numeric_limits<S>::max() <= std::numeric_limits<T>::max()) &&
        (std::numeric_limits<S>::min() >= std::numeric_limits<T>::min()))
{
    return static_cast<T>(value);
}

template <ieee_float T, std::integral S>
[[nodiscard]] constexpr T safe_cast(const S & value) noexcept
    requires(std::numeric_limits<T>::digits >= std::numeric_limits<S>::digits)
{
    return static_cast<T>(value);
}

template <std::signed_integral T, ieee_float S>
[[nodiscard]] constexpr T safe_ceil_cast(const S & value) noexcept
    requires(std::numeric_limits<T>::digits >= std::numeric_limits<S>::digits)
{
    return static_cast<T>(std::ceil(value));
}

template <std::signed_integral T, ieee_float S>
[[nodiscard]] constexpr T safe_floor_cast(const S & value) noexcept
    requires(std::numeric_limits<T>::digits >= std::numeric_limits<S>::digits)
{
    return static_cast<T>(std::floor(value));
}

template <std::signed_integral T, ieee_float S>
[[nodiscard]] constexpr T safe_round_cast(const S & value) noexcept
    requires(std::numeric_limits<T>::digits >= std::numeric_limits<S>::digits)
{
    return static_cast<T>(std::round(value));
}

template <typename T, typename S>
concept SafelyCastableTo = requires(S value) { safe_cast<T>(value); };

static_assert(SafelyCastableTo<f32, bool>);
static_assert(SafelyCastableTo<f32, u8>);
static_assert(SafelyCastableTo<f32, s8>);
static_assert(SafelyCastableTo<f32, u16>);
static_assert(SafelyCastableTo<f32, s16>);
static_assert(!SafelyCastableTo<f32, u32>);
static_assert(!SafelyCastableTo<f32, s32>);

static_assert(SafelyCastableTo<f64, bool>);
static_assert(SafelyCastableTo<f64, u8>);
static_assert(SafelyCastableTo<f64, s8>);
static_assert(SafelyCastableTo<f64, u16>);
static_assert(SafelyCastableTo<f64, s16>);
static_assert(SafelyCastableTo<f64, u32>);
static_assert(SafelyCastableTo<f64, s32>);
static_assert(!SafelyCastableTo<f64, u64>);
static_assert(!SafelyCastableTo<f64, s64>);

static_assert(SafelyCastableTo<f64, f32>);
static_assert(!SafelyCastableTo<f32, f64>);

} // namespace ka
