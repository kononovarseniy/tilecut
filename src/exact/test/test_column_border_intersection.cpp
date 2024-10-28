#include <gtest/gtest.h>

#include <array>
#include <cmath>
#include <format>
#include <optional>

#include <mpfr.h>

#include <common/assert.hpp>
#include <common/fixed.hpp>
#include <exact/generated/constants.hpp>
#include <exact/grid.hpp>

#include "check_column_border_intersection.hpp"

namespace r7
{

TEST(ColumnBorderIntersectionTest, combinations_of_dangerous_values)
{
    mpfr_t value;
    mpfr_init2(value, 53);
    const auto round_to_cell = [&value](const f64 x, const f64 size, const mpfr_rnd_t rnd) -> s64
    {
        AR_ASSERT(0 == mpfr_set_d(value, x, rnd));
        mpfr_div_d(value, value, size, rnd);
        return mpfr_get_si(value, rnd);
    };

    const std::array coordinates {
        -g_max_regular_grid_input_coordinate,
        -std::nextafter(g_max_regular_grid_input_coordinate, 0.0),

        -std::nextafter(g_min_grid_step, std::numeric_limits<f64>::infinity()),
        -g_min_grid_step,
        -std::nextafter(g_min_grid_step, 0.0),

        -std::nextafter(g_min_regular_grid_input_coordinate, std::numeric_limits<f64>::infinity()),
        -g_min_regular_grid_input_coordinate,

        0.0,

        +g_min_regular_grid_input_coordinate,
        +std::nextafter(g_min_regular_grid_input_coordinate, std::numeric_limits<f64>::infinity()),

        +std::nextafter(g_min_grid_step, 0.0),
        +g_min_grid_step,
        +std::nextafter(g_min_grid_step, std::numeric_limits<f64>::infinity()),

        +std::nextafter(g_max_regular_grid_input_coordinate, 0.0),
        +g_max_regular_grid_input_coordinate,
    };

    size_t case_index = 0;
    for (const auto size : { g_min_grid_step, 2.0 * g_min_grid_step })
    {
        for (const auto a_x : coordinates)
        {
            for (const auto b_x : coordinates)
            {
                if (a_x == b_x)
                {
                    continue;
                }
                const auto min_c_x = round_to_cell(std::min(a_x, b_x), size, MPFR_RNDU);
                const auto max_c_x = round_to_cell(std::max(a_x, b_x), size, MPFR_RNDD);
                if (min_c_x > max_c_x)
                {
                    continue;
                }

                for (const auto a_y : coordinates)
                {
                    for (const auto b_y : coordinates)
                    {
                        std::optional<s64> prev_c_x;
                        for (const auto c_x : { min_c_x, (min_c_x + max_c_x) / 2, max_c_x })
                        {
                            if (c_x == prev_c_x)
                            {
                                continue;
                            }
                            prev_c_x = c_x;
                            const auto c_y =
                                column_border_intersection<GridRounding::Cell>(a_x, a_y, b_x, b_y, size, c_x);
                            ASSERT_TRUE(check_column_border_intersecion(a_x, a_y, b_x, b_y, size, c_x, c_y))
                                << std::format(
                                       "#{})s: {}; a: {}, {}; b: {}, {}; c: {}, {}",
                                       case_index,
                                       size,
                                       a_x,
                                       a_y,
                                       b_x,
                                       b_y,
                                       c_x,
                                       c_y);
                            ++case_index;
                        }
                    }
                }
            }
        }
    }

    mpfr_clear(value);
}

} // namespace r7
