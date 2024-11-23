#pragma once

#include <cmath>
#include <concepts>
#include <numeric>
#include <source_location>
#include <type_traits>

#include <common/assert.hpp>
#include <common/fixed.hpp>

namespace ka
{

template <std::integral T, std::floating_point S>
[[nodiscard]] constexpr T exact_cast_float(
    const S & value,
    const std::source_location & location = std::source_location::current()) noexcept
{
    constexpr auto assert_type = "exact_cast_floating";

    AR_NESTED_ASSERT(value <= std::numeric_limits<T>::max(), assert_type, location);
    AR_NESTED_ASSERT(value >= std::numeric_limits<T>::min(), assert_type, location);
    AR_NESTED_ASSERT(static_cast<S>(static_cast<T>(value)) == value, assert_type, location);
    return static_cast<T>(value);
}

template <std::floating_point T, std::integral S>
[[nodiscard]] constexpr T exact_cast_int(
    const S & value,
    const std::source_location & location = std::source_location::current()) noexcept
{
    constexpr auto assert_type = "exact_cast_integral";

    static_assert(std::numeric_limits<S>::max() <= std::numeric_limits<T>::max());
    static_assert(std::numeric_limits<S>::min() >= std::numeric_limits<T>::lowest());

    AR_NESTED_ASSERT(static_cast<S>(static_cast<T>(value)) == value, assert_type, location);
    return static_cast<T>(value);
}

template <std::floating_point T, std::floating_point S>
[[nodiscard]] constexpr T exact_cast(
    const S & value,
    const std::source_location & location = std::source_location::current()) noexcept
{
    constexpr auto assert_type = "exact_cast [float]";

    const auto result = static_cast<T>(value);

    AR_NESTED_ASSERT(result == value, assert_type, location);
    return result;
}

template <std::integral T, std::integral S>
[[nodiscard]] constexpr T exact_cast(
    const S & value,
    const std::source_location & location = std::source_location::current()) noexcept
{
    constexpr auto assert_type = "exact_cast [int]";

    constexpr auto same_signedness = std::is_signed_v<S> == std::is_signed_v<T>;
    const auto result = static_cast<T>(value);

    AR_NESTED_ASSERT(same_signedness || (result > T {}) == (value > S {}), assert_type, location);
    using CommonType = decltype(result + value);
    AR_NESTED_ASSERT(static_cast<CommonType>(result) == static_cast<CommonType>(value), assert_type, location);
    return result;
}

template <std::integral T, std::integral S>
[[nodiscard]] constexpr T safe_cast(const S & value) noexcept
    requires(
        (std::numeric_limits<S>::max() <= std::numeric_limits<T>::max()) &&
        (std::numeric_limits<S>::min() >= std::numeric_limits<T>::min()))
{
    return static_cast<T>(value);
}

template <std::floating_point T, std::floating_point S>
[[nodiscard]] constexpr T safe_cast(const S & value) noexcept
    requires(
        (std::numeric_limits<S>::max() <= std::numeric_limits<T>::max()) &&
        (std::numeric_limits<S>::min() >= std::numeric_limits<T>::min()))
{
    return static_cast<T>(value);
}

template <std::floating_point T, std::integral S>
[[nodiscard]] constexpr T safe_cast(const S & value) noexcept
    requires(std::numeric_limits<T>::is_iec559 && std::numeric_limits<T>::digits >= std::numeric_limits<S>::digits)
{
    return static_cast<T>(value);
}

template <std::signed_integral T, std::floating_point S>
[[nodiscard]] constexpr T safe_ceil_cast(const S & value) noexcept
    requires(std::numeric_limits<S>::is_iec559 && std::numeric_limits<T>::digits >= std::numeric_limits<S>::digits)
{
    return static_cast<T>(std::ceil(value));
}

template <std::signed_integral T, std::floating_point S>
[[nodiscard]] constexpr T safe_floor_cast(const S & value) noexcept
    requires(std::numeric_limits<S>::is_iec559 && std::numeric_limits<T>::digits >= std::numeric_limits<S>::digits)
{
    return static_cast<T>(std::floor(value));
}

template <std::signed_integral T, std::floating_point S>
[[nodiscard]] constexpr T safe_round_cast(const S & value) noexcept
    requires(std::numeric_limits<S>::is_iec559 && std::numeric_limits<T>::digits >= std::numeric_limits<S>::digits)
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
