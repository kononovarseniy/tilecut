#include <cmath>

#include <common/assert.hpp>
#include <common/cast.hpp>
#include <common/fixed.hpp>
#include <exact/generated/constants.hpp>
#include <exact/grid.hpp>

#include "../expansion.hpp"
#include "common.hpp"

namespace r7
{

s64 column_border_intersecion_impl(
    const f64 a_x,
    const f64 a_y,
    const f64 b_x,
    const f64 b_y,
    const f64 size,
    const s64 c_x) noexcept
{
    AR_PRE(a_x != b_x);
    AR_PRE(border_between_coordinates(a_x, b_x, size, c_x));
    AR_PRE(a_x == 0.0 || std::abs(a_x) >= g_min_regular_grid_input_coordinate);
    AR_PRE(a_y == 0.0 || std::abs(a_y) >= g_min_regular_grid_input_coordinate);
    AR_PRE(b_x == 0.0 || std::abs(b_x) >= g_min_regular_grid_input_coordinate);
    AR_PRE(b_y == 0.0 || std::abs(b_y) >= g_min_regular_grid_input_coordinate);
    AR_PRE(std::abs(a_x) <= g_max_regular_grid_input_coordinate);
    AR_PRE(std::abs(a_y) <= g_max_regular_grid_input_coordinate);
    AR_PRE(std::abs(b_x) <= g_max_regular_grid_input_coordinate);
    AR_PRE(std::abs(b_y) <= g_max_regular_grid_input_coordinate);
    AR_PRE(size >= g_min_grid_step);

    // The fused operation is probably not required here, but it makes error analysis easier.
    const auto t_numerator = std::fma(c_x, size, -a_x);
    const auto t_denominator = b_x - a_x;
    const auto t = t_numerator / t_denominator;
    const auto delta = b_y - a_y;
    const auto lerp_delta = delta * t;
    const auto lerp = a_y + lerp_delta;
    const auto intersection = lerp / size;

    /// Checks if a value can be the result of rounding a quotient towards negative infinity.
    const auto check_value = [&a_x, &a_y, &b_x, &b_y, &size, &c_x](const f64 value)
    {
        /// a_y * b_x - a_x * b_y
        std::array<f64, 4> numerator_1;
        expansion_diff(const_span(two_product(a_y, b_x)), const_span(two_product(a_x, b_y)), span(numerator_1));
        /// size * (b_y - a_y)
        std::array<f64, 4> size_dy;
        scale_expansion(const_span(two_diff(b_y, a_y)), size, span(size_dy));
        /// c_x * size * (b_y - a_y)
        std::array<f64, 8> numerator_2;
        scale_expansion(const_span(size_dy), exact_cast_int<f64>(c_x), span(numerator_2));

        std::array<f64, 12> numerator;
        // TODO: fast_expansion_sum
        expansion_sum(const_span(numerator_1), const_span(numerator_2), span(numerator));

        /// size * (b_x - a_x)
        std::array<f64, 4> denominator;
        scale_expansion(const_span(two_diff(b_x, a_x)), size, span(denominator));

        /// value * denominator
        std::array<f64, 8> product;
        scale_expansion(const_span(denominator), value, span(product));
        /// value * denominator - numerator
        std::array<f64, 20> difference;
        // TODO: fast_expansion_diff
        expansion_diff(const_span(product), const_span(numerator), span(difference));

        const auto difference_sign = expansion_approx(const_span(difference));
        const auto positive_denominator = b_x > a_x;
        return positive_denominator ? difference_sign <= 0.0 : difference_sign >= 0.0;
    };

    // Computation of 1.0 - fractional_part may be inexact,
    // so we can't avoid branching based on the sign of the value using std::floor.
    f64 integral_part;
    const auto fractional_part = std::abs(std::modf(intersection, &integral_part));
    s64 truncated = exact_cast_float<s64>(integral_part);

    if (intersection >= 0.0)
    {
        if (fractional_part < column_border_intersecion_detail::g_min_reliable_fractional_part &&
            !check_value(integral_part))
        {
            return truncated - 1;
        }
        if (fractional_part > column_border_intersecion_detail::g_max_reliable_fractional_part &&
            check_value(integral_part + 1.0))
        {
            return truncated + 1;
        }
        return truncated;
    }

    if (fractional_part > column_border_intersecion_detail::g_max_reliable_fractional_part &&
        !check_value(integral_part - 1.0))
    {
        return truncated - 2;
    }
    if (fractional_part < column_border_intersecion_detail::g_min_reliable_fractional_part &&
        check_value(integral_part))
    {
        return truncated;
    }
    return truncated - 1;
}

template <GridRounding rounding>
s64 column_border_intersection(
    const f64 a_x,
    const f64 a_y,
    const f64 b_x,
    const f64 b_y,
    const f64 size,
    const s64 c_x) noexcept
{
    const auto c_y = [&]
    {
        switch (rounding)
        {
        case GridRounding::Cell:
            return column_border_intersecion_impl(a_x, a_y, b_x, b_y, size, c_x);
        case GridRounding::NearestNode:
            return half_cell_to_nearest_full_cell(
                column_border_intersecion_impl(a_x, a_y, b_x, b_y, size / 2.0, c_x * 2.0));
        }
        AR_UNREACHABLE;
    }();
    AR_POST(line_intersects_cell<rounding>(a_x, a_y, b_x, b_y, size, c_x, c_y));
    return c_y;
}

template s64 column_border_intersection<GridRounding::Cell>(f64 a_x, f64 a_y, f64 b_x, f64 b_y, f64 size, s64 c_x);
template s64 column_border_intersection<
    GridRounding::NearestNode>(f64 a_x, f64 a_y, f64 b_x, f64 b_y, f64 size, s64 c_x);

template <GridRounding rounding>
s64 row_border_intersection(
    const f64 a_x,
    const f64 a_y,
    const f64 b_x,
    const f64 b_y,
    const f64 size,
    const s64 c_y) noexcept
{
    const auto c_x = [&]
    {
        switch (rounding)
        {
        case GridRounding::Cell:
            return column_border_intersecion_impl(-a_y, a_x, -b_y, b_x, size, -c_y);
        case GridRounding::NearestNode:
            return half_cell_to_nearest_full_cell(
                column_border_intersecion_impl(-a_y, a_x, -b_y, b_x, size / 2.0, -c_y * 2));
        }
        AR_UNREACHABLE;
    }();
    AR_POST(line_intersects_cell<rounding>(a_x, a_y, b_x, b_y, size, c_x, c_y));
    return c_x;
}

template s64 row_border_intersection<GridRounding::Cell>(f64 a_x, f64 a_y, f64 b_x, f64 b_y, f64 size, s64 c_y);
template s64 row_border_intersection<GridRounding::NearestNode>(f64 a_x, f64 a_y, f64 b_x, f64 b_y, f64 size, s64 c_y);

} // namespace r7
