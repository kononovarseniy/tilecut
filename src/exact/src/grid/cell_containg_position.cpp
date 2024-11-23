#include <cmath>

#include <ka/common/assert.hpp>
#include <ka/common/cast.hpp>
#include <ka/common/fixed.hpp>
#include <ka/exact/generated/constants.hpp>
#include <ka/exact/grid.hpp>

#include "../expansion.hpp"
#include "common.hpp"

namespace ka
{

s64 column_containing_position_impl(const f64 x, const f64 size) noexcept
{
    AR_PRE(std::abs(x) <= g_max_regular_grid_input_coordinate);
    AR_PRE(size >= g_min_grid_step);

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
            return exact_cast_float<s64>(candidate) - 1;
        }
    }
    return exact_cast_float<s64>(candidate);
}

template <GridRounding rounding>
s64 column_containing_position(const f64 x, const f64 size) noexcept
{
    switch (rounding)
    {
    case GridRounding::Cell:
        return column_containing_position_impl(x, size);
    case GridRounding::NearestNode:
        return half_cell_to_nearest_full_cell(column_containing_position_impl(x, size / 2.0));
    }
    AR_UNREACHABLE;
}

template s64 column_containing_position<GridRounding::Cell>(f64 x, f64 size);
template s64 column_containing_position<GridRounding::NearestNode>(f64 x, f64 size);

template <GridRounding rounding>
s64 row_containing_position(const f64 y, const f64 size) noexcept
{
    return column_containing_position<rounding>(y, size);
}

template s64 row_containing_position<GridRounding::Cell>(f64 y, f64 size);
template s64 row_containing_position<GridRounding::NearestNode>(f64 y, f64 size);

} // namespace ka
