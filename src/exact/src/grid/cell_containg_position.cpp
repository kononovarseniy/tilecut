#include <cmath>

#include <ka/common/assert.hpp>
#include <ka/common/cast.hpp>
#include <ka/common/fixed.hpp>
#include <ka/exact/grid.hpp>

#include "../expansion.hpp"
#include "common.hpp"

namespace ka
{

/// Size is used to override grid.cell_size without modifying struct.
s64 column_containing_position_impl([[maybe_unused]] const GridParameters & grid, const f64 size, const f64 x) noexcept
{
    AR_PRE(std::abs(x) <= grid.max_input);
    AR_PRE(grid.desired_cell_size > 0.0);
    AR_PRE(size >= grid.desired_cell_size);

    const auto quotient = x / size;
    const auto candidate = std::floor(quotient);
    if (candidate == quotient)
    {
        // The quotient may have been rounded towards infinity,
        // so the result needs to be checked exactly.
        std::array<f64, 3> difference;
        grow_expansion(const_span(two_product(candidate, size)), -x, span(difference));
        const auto sign = expansion_approx(const_span(difference));
        // candidate * size > x
        if (sign > 0.0)
        {
            return exact_cast<s64>(candidate) - 1;
        }
    }
    return exact_cast<s64>(candidate);
}

template <GridRounding rounding>
s64 column_containing_position(const GridParameters & grid, const f64 x) noexcept
{
    switch (rounding)
    {
    case GridRounding::Cell:
        return column_containing_position_impl(grid, grid.cell_size, x);
    case GridRounding::NearestNode:
        return half_cell_to_nearest_full_cell(column_containing_position_impl(grid, grid.cell_size / 2.0, x));
    }
    AR_UNREACHABLE;
}

template s64 column_containing_position<GridRounding::Cell>(const GridParameters & grid, f64 x);
template s64 column_containing_position<GridRounding::NearestNode>(const GridParameters & grid, f64 x);

template <GridRounding rounding>
s64 row_containing_position(const GridParameters & grid, const f64 y) noexcept
{
    return column_containing_position<rounding>(grid, y);
}

template s64 row_containing_position<GridRounding::Cell>(const GridParameters & grid, f64 y);
template s64 row_containing_position<GridRounding::NearestNode>(const GridParameters & grid, f64 y);

} // namespace ka
