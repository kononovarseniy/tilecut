#include "check_column_border_intersection.hpp"

#include <mpfr.h>

#include <common/assert.hpp>

namespace r7
{

bool check_column_border_intersecion(
    const f64 a_x,
    const f64 a_y,
    const f64 b_x,
    const f64 b_y,
    const f64 size,
    const s64 c_x,
    const s64 c_y)
{
    // c_y <= lerp(a_y, b_y, t) / size < (c_y + 1)
    // t = (c_x * size - a_x) / (b_x - a_x)
    // c_y <= (a_y * b_x - a_x * b_y + c_x * size * (b_y - a_y)) / (size * (b_x - a_x)) <= c_y + 1

    constexpr auto precision = 1024;

    /// abs(size * (b_x - a_x)).
    mpfr_t abs_denom;
    mpfr_init2(abs_denom, precision);
    AR_ASSERT(0 == mpfr_set_d(abs_denom, b_x, MPFR_RNDN));
    AR_ASSERT(0 == mpfr_sub_d(abs_denom, abs_denom, a_x, MPFR_RNDN));
    AR_ASSERT(0 == mpfr_mul_d(abs_denom, abs_denom, size, MPFR_RNDN));
    AR_ASSERT(0 == mpfr_abs(abs_denom, abs_denom, MPFR_RNDN));

    /// c_y * abs(denom).
    mpfr_t lower_bound;
    mpfr_init2(lower_bound, precision);
    AR_ASSERT(0 == mpfr_set_si(lower_bound, c_y, MPFR_RNDN));
    AR_ASSERT(0 == mpfr_mul(lower_bound, lower_bound, abs_denom, MPFR_RNDN));

    /// (c_y + 1) * abs(denom).
    mpfr_t upper_bound;
    mpfr_init2(upper_bound, precision);
    AR_ASSERT(0 == mpfr_set_si(upper_bound, c_y, MPFR_RNDN));
    AR_ASSERT(0 == mpfr_add_ui(upper_bound, upper_bound, 1, MPFR_RNDN));
    AR_ASSERT(0 == mpfr_mul(upper_bound, upper_bound, abs_denom, MPFR_RNDN));

    mpfr_t tmp;
    mpfr_init2(tmp, precision);
    AR_ASSERT(0 == mpfr_set_d(tmp, a_x, MPFR_RNDN));
    AR_ASSERT(0 == mpfr_mul_d(tmp, tmp, b_y, MPFR_RNDN));

    /// sign(denom) * (a_y * b_x - a_x * b_y + c_x * size * (b_y - a_y)).
    mpfr_t num;
    mpfr_init2(num, precision);
    AR_ASSERT(0 == mpfr_set_d(num, a_y, MPFR_RNDN));
    AR_ASSERT(0 == mpfr_mul_d(num, num, b_x, MPFR_RNDN));
    AR_ASSERT(0 == mpfr_sub(num, num, tmp, MPFR_RNDN));

    AR_ASSERT(0 == mpfr_set_d(tmp, b_y, MPFR_RNDN));
    AR_ASSERT(0 == mpfr_sub_d(tmp, tmp, a_y, MPFR_RNDN));
    AR_ASSERT(0 == mpfr_mul_d(tmp, tmp, size, MPFR_RNDN));
    AR_ASSERT(0 == mpfr_mul_si(tmp, tmp, c_x, MPFR_RNDN));

    AR_ASSERT(0 == mpfr_add(num, num, tmp, MPFR_RNDN));
    const auto negative_denom = b_x < a_x;
    if (negative_denom)
    {
        AR_ASSERT(0 == mpfr_neg(num, num, MPFR_RNDN));
    }

    const auto result = mpfr_lessequal_p(lower_bound, num) && mpfr_less_p(num, upper_bound);

    mpfr_clears(abs_denom, lower_bound, upper_bound, tmp, num, nullptr);

    return result;
}

} // namespace r7
