#pragma once

#include <concepts>

#include <ka/common/fixed.hpp>

namespace ka
{

namespace detail
{

template <ieee_float F>
    requires std::same_as<F, f32> || std::same_as<F, f64>
[[nodiscard]] F orientation_impl(F a_x, F a_y, F b_x, F b_y, F c_x, F c_y) noexcept;

template <std::integral I>
    requires std::same_as<I, u16> || std::same_as<I, s16>
[[nodiscard]] s64 orientation_impl(I a_x, I a_y, I b_x, I b_y, I c_x, I c_y) noexcept;

} // namespace detail

/// @brief Computes the orientation of three ordered points (a, b, c).
/// @return Positive value if points are counter-clockwise (left turn), zero if collinear, negative if clockwise (right
/// turn).
template <typename T>
[[nodiscard]] auto orientation(const T a_x, const T a_y, const T b_x, const T b_y, const T c_x, const T c_y) noexcept
{
    return detail::orientation_impl(a_x, a_y, b_x, b_y, c_x, c_y);
}

} // namespace ka
