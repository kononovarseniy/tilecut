#include <array>
#include <concepts>

#include <ka/exact/orientation.hpp>

#include "common.hpp"
#include "expansion.hpp"

namespace ka
{

namespace detail
{

template <ieee_float F>
    requires std::same_as<F, f32> || std::same_as<F, f64>
[[nodiscard]] F orientation_impl(const F a_x, const F a_y, const F b_x, const F b_y, const F c_x, const F c_y) noexcept
{
    // TODO: Implement robust predicate from [Shewchuk].
    //
    // | b_x - a_x   c_x - a_x |
    // |                       | = (b_x - a_x)(c_y - a_y) - (b_y - a_y)(c_x - a_x) =
    // | b_y - a_y   c_y - a_y |
    //
    // = (b_x - a_x) * c_y - (b_x - a_x) * a_y - (b_y - a_y) * c_x + (b_y - a_y) * a_x

    const auto dx = two_diff(b_x, a_x);
    const auto dy = two_diff(b_y, a_y);

    std::array<F, 4> t1;
    scale_expansion(const_span(dx), c_y, span(t1));
    std::array<F, 4> t2;
    scale_expansion(const_span(dx), a_y, span(t2));
    std::array<F, 4> t3;
    scale_expansion(const_span(dy), c_x, span(t3));
    std::array<F, 4> t4;
    scale_expansion(const_span(dy), a_x, span(t4));

    std::array<F, 8> t1t4;
    fast_expansion_sum(const_span(t1), const_span(t4), span(t1t4));
    std::array<F, 8> t2t3;
    fast_expansion_sum(const_span(t2), const_span(t3), span(t2t3));

    std::array<F, 16> res;
    fast_expansion_difference(const_span(t1t4), const_span(t2t3), span(res));

    return expansion_approx(const_span(res));
}

template <std::integral I>
    requires std::same_as<I, u16> || std::same_as<I, s16>
[[nodiscard]] s64 orientation_impl(
    const I a_x,
    const I a_y,
    const I b_x,
    const I b_y,
    const I c_x,
    const I c_y) noexcept
{
    const s64 m00 = b_x - a_x;
    const s64 m01 = c_x - a_x;
    const s64 m10 = b_y - a_y;
    const s64 m11 = c_y - a_y;
    return m00 * m11 - m01 * m10;
}

template f32 orientation_impl<f32>(f32 a_x, f32 a_y, f32 b_x, f32 b_y, f32 c_x, f32 c_y) noexcept;
template f64 orientation_impl<f64>(f64 a_x, f64 a_y, f64 b_x, f64 b_y, f64 c_x, f64 c_y) noexcept;
template s64 orientation_impl<u16>(u16 a_x, u16 a_y, u16 b_x, u16 b_y, u16 c_x, u16 c_y) noexcept;
template s64 orientation_impl<s16>(s16 a_x, s16 a_y, s16 b_x, s16 b_y, s16 c_x, s16 c_y) noexcept;

} // namespace detail

} // namespace ka