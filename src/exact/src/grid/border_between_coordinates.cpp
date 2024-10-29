#include <functional>

#include <common/assert.hpp>
#include <common/cast.hpp>
#include <common/fixed.hpp>

#include "../expansion.hpp"
#include "common.hpp"

namespace r7
{

bool border_between_coordinates(const f64 a, const f64 b, const f64 size, const s64 x) noexcept
{
    AR_PRE(a != b);
    const auto compare_border_and_coordinate = [](const f64 s, const s64 n, const f64 x, const auto cmp)
    {
        std::array<f64, 3> fms;
        const auto tmp = two_product(exact_cast_int<f64>(n), s);
        grow_expansion(const_span(tmp), -x, span(fms));
        return cmp(expansion_approx(const_span(fms)), 0.0);
    };
    if (a < b)
    {
        return compare_border_and_coordinate(size, x, a, std::greater_equal<f64> {}) &&
               compare_border_and_coordinate(size, x, b, std::less_equal<f64> {});
    }
    else
    {
        return compare_border_and_coordinate(size, x, a, std::less_equal<f64> {}) &&
               compare_border_and_coordinate(size, x, b, std::greater_equal<f64> {});
    }
}

} // namespace r7