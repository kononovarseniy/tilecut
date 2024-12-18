#include <array>
#include <span>
#include <utility>

#include <ka/common/assert.hpp>
#include <ka/common/cast.hpp>
#include <ka/common/fixed.hpp>
#include <ka/exact/grid.hpp>

#include "../expansion.hpp"
#include "common.hpp"

namespace ka
{

inline namespace
{

/// @brief One always can choose a diagonal of the cell that intersects the given line iff the line intersects the cell.
/// The line intersects the diagonal iff enpoints of the choosen diagonal lie on the opposite sides of the line.
/// To find on wich side of the line the endpoint is located, we a checking the sign of the determinant
///               | a.x - size * n   a.y - size * m |
///     D(n, m) = | b.x - size * n   b.y - size * m |,
/// where n and m are coordinates of the cell on the grid.
///
/// With simple conversions more convenient formula for the determinant can be obtained
///     D(n, m) = (a_x * b_y - a_y * b_x) + size * (n * (a.y - b.y) - m * (a.x - b.x)).
///
/// One can note that the difference of determinants difference_term does not depend on the coordinates of the cell
///                       { D(n + 1, m) - D(n, m + 1) = size * ((a.y - b.y) + (a.x - b.x)),
///     difference_term = { D(n + 1, m + 1) - D(n, m) = size * ((a.y - b.y) - (a.x - b.x)).
///
/// Let
///     first_determinant = common_term + cell_dependent_term,
///     second_determinant = common_term + cell_dependent_term + difference_term,
/// where
///     common_term = a_x * b_y - a_y * b_x,
///     cell_dependent_term = size * (n * (a.y - b.y) - m * (a.x - b.x)).
///
/// There is the possibility of reusing some terms and thair sum when testing the same line against multiple cells.
/// But testing a line against a single cell appears to be more efficient when using lazy evaluation of terms.
namespace line_cell_intersection
{

struct Flags final
{
    /// @brief When signs are not inverted intersection requires first_determinant < 0 && second_determinant > 0.
    bool invert_signs;
    bool main_diagonal;
};

[[nodiscard]] Flags choose_flags(const f64 a_x, const f64 a_y, const f64 b_x, const f64 b_y) noexcept
{
    //! The main diagonal of a cell is a segment parallel to the line x = y.
    const bool main_diagonal = (a_x <= b_x && a_y >= b_y) || (a_x >= b_x && a_y <= b_y);
    if (main_diagonal)
    {
        return {
            .invert_signs = a_x >= b_x && a_y <= b_y,
            .main_diagonal = main_diagonal,
        };
    }
    return {
        .invert_signs = a_x < b_x,
        .main_diagonal = main_diagonal,
    };
}

/// @brief Common term of two determinants in the form of a 4-component non-adjacent expansion.
[[nodiscard]] inline std::array<f64, 4> common_term(const f64 a_x, const f64 a_y, const f64 b_x, const f64 b_y) noexcept
{
    /// a_x * b_y - a_y * b_x
    std::array<f64, 4> term;
    expansion_diff(const_span(two_product(a_x, b_y)), const_span(two_product(a_y, b_x)), span(term));
    return term;
}

/// @brief Difference between two determinants in the form of a 8-component non-adjacent expansion.
[[nodiscard]] inline std::array<f64, 8> difference_term(
    const bool main_diagonal,
    const f64 size,
    const std::span<const f64, 2> dx,
    const std::span<const f64, 2> dy) noexcept
{
    /// size * (dy -+ dx)
    std::array<f64, 8> term;
    std::array<f64, 4> tmp;
    if (main_diagonal)
    {
        expansion_diff(dy, dx, span(tmp));
    }
    else
    {
        expansion_sum(dy, dx, span(tmp));
    }
    scale_expansion(const_span(tmp), size, span(term));
    return term;
}

struct CellNode final
{
    s64 x;
    s64 y;
    /// Power of two.
    f64 size_multiplier;
};

template <GridRounding rounding>
[[nodiscard]] inline CellNode main_cell_node(const bool main_diagonal, const s64 c_x, const s64 c_y) noexcept
{
    /// For snapping to grid nodes we internaly use rounding to cell to reduce number of possibly inexact operations.
    if constexpr (rounding == GridRounding::Cell)
    {
        AR_PRE(c_y <= std::numeric_limits<decltype(c_y)>::max() - 1);
        return { c_x, main_diagonal ? c_y : c_y + 1, 1.0 };
    }
    if constexpr (rounding == GridRounding::NearestNode)
    {
        // For this rounding mode we are using smaller cell size but bigger cell coordinates.
        // The distance between corners remains the same, so does difference_term.
        AR_PRE(c_x >= (std::numeric_limits<decltype(c_x)>::min() + 1) / 2);
        AR_PRE(c_y <= (std::numeric_limits<decltype(c_y)>::max() - 1) / 2);
        return { c_x * 2 - 1, main_diagonal ? c_y * 2 - 1 : c_y * 2 + 1, 0.5 };
    }
}

/// @brief The second common term of both determinants depending on the cell coordinates in the form of a 16-component
/// non-adjacent expansion.
[[nodiscard]] inline std::array<f64, 16> cell_dependent_term(
    const s64 node_x,
    const s64 node_y,
    const f64 size,
    const std::span<const f64, 2> dx,
    const std::span<const f64, 2> dy) noexcept
{
    const auto n = exact_cast_int<f64>(node_x);
    const auto m = exact_cast_int<f64>(node_y);

    std::array<f64, 16> term;
    std::array<f64, 4> ndy;
    scale_expansion(dy, n, span(ndy));
    std::array<f64, 4> mdx;
    scale_expansion(dx, m, span(mdx));
    std::array<f64, 8> cell_tmp;
    expansion_diff(const_span(ndy), const_span(mdx), span(cell_tmp));
    scale_expansion(const_span(cell_tmp), size, span(term));
    return term;
}

/// @brief Approximate value of the first determinant with the same sign as determinant.
[[nodiscard]] inline f64 first_determinant_sign(
    const std::span<const f64, 4> common_term,
    const std::span<const f64, 16> cell_dependent_term) noexcept
{
    std::array<f64, 20> first_determinant;
    fast_expansion_sum(common_term, cell_dependent_term, span(first_determinant));
    return expansion_approx(const_span(first_determinant));
}

/// @brief Approximate value of the second determinant with the same sign as determinant.
[[nodiscard]] inline f64 second_determinant_sign(
    const std::span<const f64, 4> common_term,
    const std::span<const f64, 8> difference_term,
    const std::span<const f64, 16> cell_dependent_term) noexcept
{
    /// common_term + difference_term
    std::array<f64, 12> precomputed_second;
    expansion_sum(common_term, difference_term, span(precomputed_second));
    std::array<f64, 28> second_determinant;
    fast_expansion_sum(const_span(precomputed_second), cell_dependent_term, span(second_determinant));
    return expansion_approx(const_span(second_determinant));
}

[[nodiscard]] inline bool good_first_sign(const bool invert_signs, const f64 first_sign) noexcept
{
    return invert_signs ? first_sign > 0 : first_sign < 0;
}

[[nodiscard]] inline bool good_second_sign(const bool invert_signs, const f64 second_sign) noexcept
{
    return invert_signs ? second_sign < 0 : second_sign > 0;
}

} // namespace line_cell_intersection

} // namespace

template <GridRounding rounding>
bool line_intersects_cell(
    const GridParameters & grid,
    const f64 a_x,
    const f64 a_y,
    const f64 b_x,
    const f64 b_y,
    const s64 c_x,
    const s64 c_y) noexcept
{
    const auto [invert_signs, main_diagonal] = line_cell_intersection::choose_flags(a_x, a_y, b_x, b_y);

    const auto dx = two_diff(a_x, b_x);
    const auto dy = two_diff(a_y, b_y);

    const auto common_term = line_cell_intersection::common_term(a_x, a_y, b_x, b_y);

    const auto node = line_cell_intersection::main_cell_node<rounding>(main_diagonal, c_x, c_y);
    const auto cell_dependent_term =
        line_cell_intersection::cell_dependent_term(node.x, node.y, node.size_multiplier * grid.cell_size, dx, dy);

    const auto first_sign = line_cell_intersection::first_determinant_sign(common_term, cell_dependent_term);
    if (main_diagonal && first_sign == 0)
    {
        return true;
    }
    if (!line_cell_intersection::good_first_sign(invert_signs, first_sign))
    {
        return false;
    }

    const auto difference_term = line_cell_intersection::difference_term(main_diagonal, grid.cell_size, dx, dy);
    const auto second_sign =
        line_cell_intersection::second_determinant_sign(common_term, difference_term, cell_dependent_term);
    return line_cell_intersection::good_second_sign(invert_signs, second_sign);
}

template bool line_intersects_cell<
    GridRounding::Cell>(const GridParameters & grid, f64 a_x, f64 a_y, f64 b_x, f64 b_y, s64 c_x, s64 c_y);
template bool line_intersects_cell<
    GridRounding::NearestNode>(const GridParameters & grid, f64 a_x, f64 a_y, f64 b_x, f64 b_y, s64 c_x, s64 c_y);

} // namespace ka
