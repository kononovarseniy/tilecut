#include "exact/grid.hpp"

#include <cmath>
#include <span>

#include <common/cast.hpp>
#include <common/fixed.hpp>
#include <exact/grid.hpp>

#include "expansion.hpp"

namespace r7
{

inline namespace
{

template <typename T, size_t E>
[[nodiscard]] constexpr auto span(std::array<T, E> & array) noexcept
{
    return std::span<T, E> { array };
}

template <typename T, size_t E>
[[nodiscard]] constexpr auto const_span(const std::array<T, E> & array) noexcept
{
    return std::span<const T, E> { array };
}

} // namespace

bool line_intersects_cell(
    const f64 a_x,
    const f64 a_y,
    const f64 b_x,
    const f64 b_y,
    const f64 size,
    const s64 c_x,
    const s64 c_y) noexcept
{
    // One always can choose a diagonal of the cell that intersects the given line iff the line intersects the cell.
    // The line intersects the diagonal iff enpoints of the choosen diagonal lie on the opposite sides of the line.
    // To find on wich side of the line the corner point is located,
    // one can determine the sign of the determinant
    //               | a.x - size * n   a.y - size * m |
    //     D(n, m) = | b.x - size * n   b.y - size * m |,
    // where n and m are coordinates of the cell corner on the grid.
    // With simple conversions more convenient formula for the determinant can be obtained
    //     D(n, m) = (a_x * b_y - a_y * b_x) + size * (n * (a.y - b.y) - m * (a.x - b.x)).
    // One can note that the difference of determinants corner_delta does not depend on the coordinates of the cell
    //                    { D(n + 1, m) - D(m, m + 1) = size * ((a.y - b.y) + (a.x - b.x)),
    //     corner_delta = { D(n + 1, m + 1) - D(n, m) = size * ((a.y - b.y) - (a.x - b.x)).
    // Let
    //     cell_dependent_term = size * (n * (a.y - b.y) - m * (a.x - b.x)),
    //     precomputed_first = (a_x * b_y - a_y * b_x)
    //     precomputed_second = precomputed_first + corner_delta
    // then determinants for enpoints of the line can be expressed as
    //     first_determinant = precomputed_first + cell_dependent_part,
    //     second_determinant = precomputed_second + cell_dependent_part,
    // This opens the possibility of reusing values ​​when testing the same line against multiple cells..
    const bool left_to_right = a_x < b_x;
    const bool intersect_with_main_diagonal = (a_x < b_x && a_y > b_y) || (a_x > b_x && a_y < b_y);

    const auto dx = two_diff(a_x, b_x);
    const auto dy = two_diff(a_y, b_y);

    /// a_x * b_y - a_y * b_x
    std::array<f64, 4> precomputed_first;
    expansion_diff(two_product(a_x, b_y).span(), two_product(a_y, b_x).span(), span(precomputed_first));

    /// size * (dy -+ dx)
    std::array<f64, 8> corners_delta;
    std::array<f64, 4> tmp;
    if (intersect_with_main_diagonal)
    {
        expansion_diff(dy.span(), dx.span(), span(tmp));
    }
    else
    {
        expansion_sum(dy.span(), dx.span(), span(tmp));
    }
    scale_expansion(const_span(tmp), size, span(corners_delta));

    /// (a_x * b_y - a_y * b_x) + size * (dy -+ dx)
    std::array<f64, 12> precomputed_second;
    expansion_sum(const_span(precomputed_first), const_span(corners_delta), span(precomputed_second));

    const auto n = exact_cast_int<f64>(c_x);
    const auto m = exact_cast_int<f64>(intersect_with_main_diagonal ? c_y : c_y + 1);
    std::array<f64, 16> cell_dependent_term;
    std::array<f64, 4> ndy;
    scale_expansion(dy.span(), n, span(ndy));
    std::array<f64, 4> mdx;
    scale_expansion(dx.span(), m, span(mdx));
    std::array<f64, 8> cell_tmp;
    expansion_diff(const_span(ndy), const_span(mdx), span(cell_tmp));
    scale_expansion(const_span(cell_tmp), size, span(cell_dependent_term));

    std::array<f64, 20> first_determinant;
    fast_expansion_sum(const_span(precomputed_first), const_span(cell_dependent_term), span(first_determinant));
    const auto first_sign = expansion_approx(const_span(first_determinant));
    if (!intersect_with_main_diagonal && first_sign == 0)
    {
        return true;
    }
    if (intersect_with_main_diagonal && ((left_to_right && first_sign <= 0) || (!left_to_right && first_sign >= 0)))
    {
        return false;
    }
    if (!intersect_with_main_diagonal && ((left_to_right && first_sign >= 0) || (!left_to_right && first_sign <= 0)))
    {
        return false;
    }
    std::array<f64, 28> second_determinant;
    fast_expansion_sum(const_span(precomputed_second), const_span(cell_dependent_term), span(second_determinant));
    const auto second_sign = expansion_approx(const_span(second_determinant));
    if (intersect_with_main_diagonal && ((left_to_right && second_sign >= 0) || (!left_to_right && second_sign <= 0)))
    {
        return false;
    }
    if (!intersect_with_main_diagonal && ((left_to_right && second_sign <= 0) || (!left_to_right && second_sign >= 0)))
    {
        return false;
    }
    return true;
}

s64 column_containing_position(const f64 x, const f64 size) noexcept
{
    AR_PRE(std::abs(x) <= g_max_regular_grid_input_coordinate);
    const auto quotient = x / size;
    const auto candidate = std::floor(quotient);
    if (candidate == quotient)
    {
        // The quotient may have been rounded towards infinity,
        // so the result needs to be checked exactly.
        std::array<f64, 3> difference;
        grow_expansion(two_product(candidate, size).span(), -x, span(difference));
        const auto sign = expansion_approx(const_span(difference));
        // candidate * size > x
        if (sign > 0.0)
        {
            return exact_cast_float<s64>(candidate) - 1;
        }
    }
    return exact_cast_float<s64>(candidate);
}

s64 column_border_intersecion(
    const f64 a_x,
    const f64 a_y,
    const f64 b_x,
    const f64 b_y,
    const f64 size,
    const s64 c_x) noexcept
{
    constexpr double min_reliable_fractional_part = 0.25; // TODO: correct value
    constexpr double max_reliable_fractional_part = 0.75; // TODO: correct value
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
        expansion_diff(two_product(a_y, b_x).span(), two_product(a_x, b_y).span(), span(numerator_1));
        /// size * (b_y - a_y)
        std::array<f64, 4> size_dy;
        scale_expansion(two_diff(b_y, a_y).span(), size, span(size_dy));
        /// c_x * size * (b_y - a_y)
        std::array<f64, 8> numerator_2;
        scale_expansion(const_span(size_dy), exact_cast_int<f64>(c_x), span(numerator_2));

        std::array<f64, 12> numerator;
        // TODO: fast_expansion_sum
        expansion_sum(const_span(numerator_1), const_span(numerator_2), span(numerator));

        /// size * (b_x - a_x)
        std::array<f64, 4> denominator;
        scale_expansion(two_diff(b_x, a_x).span(), size, span(denominator));

        /// value * denominator
        std::array<f64, 8> product;
        scale_expansion(const_span(denominator), value, span(product));
        /// value * denominator - numerator
        std::array<f64, 20> difference;
        // TODO: fast_expansion_diff
        expansion_diff(const_span(product), const_span(numerator), span(difference));

        return expansion_approx(const_span(difference)) <= 0.0;
    };

    // Computation of 1.0 - fractional_part may be inexact,
    // so we can't avoid branching based on the sign of the value using std::floor.
    f64 integral_part;
    const auto fractional_part = std::abs(std::modf(intersection, &integral_part));
    s64 truncated = exact_cast_float<s64>(integral_part);

    if (intersection >= 0.0)
    {
        if (fractional_part < min_reliable_fractional_part && !check_value(integral_part))
        {
            return truncated - 1;
        }
        if (fractional_part > max_reliable_fractional_part && check_value(integral_part + 1.0))
        {
            return truncated + 1;
        }
        return truncated;
    }

    if (-fractional_part > max_reliable_fractional_part && !check_value(integral_part))
    {
        return truncated - 2;
    }
    if (-fractional_part < min_reliable_fractional_part && check_value(integral_part + 1.0))
    {
        return truncated;
    }
    return truncated - 1;
}

} // namespace r7
