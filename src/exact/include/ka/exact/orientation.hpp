#pragma once

#include <ka/common/fixed.hpp>

namespace ka
{

/// @brief Computes the orientation of three ordered points (a, b, c).
/// @return Positive value if points are counter-clockwise (left turn), zero if collinear, negative if clockwise (right
/// turn).
[[nodiscard]] f64 orientation(f64 a_x, f64 a_y, f64 b_x, f64 b_y, f64 c_x, f64 c_y) noexcept;

} // namespace ka
