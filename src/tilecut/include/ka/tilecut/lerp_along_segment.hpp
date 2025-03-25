#pragma once

#include <cmath>

#include <ka/common/fixed.hpp>
#include <ka/geometry_types/Vec2.hpp>

namespace ka
{

template <ieee_float F>
[[nodiscard]] constexpr F lerp_along_segment(
    const Vec2<F> start,
    const F start_value,
    const Vec2<F> stop,
    const F stop_value,
    const Vec2<F> position) noexcept
{
    const Vec2<F> start_to_stop { stop.x - start.x, stop.y - start.y };
    const auto start_to_stop_len_sqr = start_to_stop.x * start_to_stop.x + start_to_stop.y * start_to_stop.y;
    const Vec2<F> start_to_pos { position.x - start.x, position.y - start.y };
    const auto t = (start_to_pos.x * start_to_stop.x + start_to_pos.y * start_to_stop.y) / start_to_stop_len_sqr;
    return std::lerp(start_value, stop_value, t);
}

} // namespace ka
