#include <array>

#include <ka/exact/orientation.hpp>

#include "common.hpp"
#include "expansion.hpp"

namespace ka
{

f64 orientation(f64 a_x, f64 a_y, f64 b_x, f64 b_y, f64 c_x, f64 c_y) noexcept
{
    // TODO: Implement robust predicate from [Shewchuk].
    //
    // | b_x - a_x   c_x - a_x |
    // |                       | = (b_x - a_x)(c_y - a_y) - (b_y - a_y)(c_x - a_x) =
    // | b_y - a_y   c_y - a_y |
    //
    // = (b_x - a_x) * c_y - (b_x - a_x) * a_y - (b_y - a_y) * c_x + (b_y - a_y) * a_x

    const auto dx = fast_two_diff(b_x, a_x);
    const auto dy = fast_two_diff(b_y, a_y);

    std::array<f64, 4> t1;
    scale_expansion(const_span(dx), c_y, span(t1));
    std::array<f64, 4> t2;
    scale_expansion(const_span(dx), a_y, span(t2));
    std::array<f64, 4> t3;
    scale_expansion(const_span(dy), c_x, span(t3));
    std::array<f64, 4> t4;
    scale_expansion(const_span(dy), a_x, span(t4));

    std::array<f64, 8> t1t4;
    fast_expansion_sum(const_span(t1), const_span(t4), span(t1t4));
    std::array<f64, 8> t2t3;
    fast_expansion_sum(const_span(t2), const_span(t3), span(t2t3));

    std::array<f64, 16> res;
    fast_expansion_difference(const_span(t1t4), const_span(t2t3), span(res));

    return expansion_approx(const_span(res));
}

} // namespace ka