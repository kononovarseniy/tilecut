#include <gtest/gtest.h>

#include <array>
#include <cmath>
#include <format>
#include <optional>

#include <mpfr.h>

#include <ka/common/fixed.hpp>
#include <ka/exact/grid.hpp>

#include "check_column_border_intersection.hpp"
#include "mock_grid_parameters.hpp"

namespace ka
{

TEST(ColumnBorderIntersectionTest, combinations_of_dangerous_values)
{
    mpfr_t value;
    mpfr_init2(value, 53);
    const auto round_to_cell = [&value](const f64 x, const f64 size, const mpfr_rnd_t rnd) -> s64
    {
        EXPECT_EQ(0, mpfr_set_d(value, x, rnd));
        mpfr_div_d(value, value, size, rnd);
        return mpfr_get_si(value, rnd);
    };

    const std::array coordinates {
        -g_embedded_grid.max_input,
        -std::nextafter(g_embedded_grid.max_input, 0.0),

        -std::nextafter(g_embedded_grid.cell_size, std::numeric_limits<f64>::infinity()),
        -g_embedded_grid.cell_size,
        -std::nextafter(g_embedded_grid.cell_size, 0.0),

        -std::nextafter(g_embedded_grid.min_input, std::numeric_limits<f64>::infinity()),
        -g_embedded_grid.min_input,

        0.0,

        +g_embedded_grid.min_input,
        +std::nextafter(g_embedded_grid.min_input, std::numeric_limits<f64>::infinity()),

        +std::nextafter(g_embedded_grid.cell_size, 0.0),
        +g_embedded_grid.cell_size,
        +std::nextafter(g_embedded_grid.cell_size, std::numeric_limits<f64>::infinity()),

        +std::nextafter(g_embedded_grid.max_input, 0.0),
        +g_embedded_grid.max_input,
    };

    size_t case_index = 0;
    for (const auto size : { g_embedded_grid.cell_size, 2.0 * g_embedded_grid.cell_size })
    {
        auto grid = g_embedded_grid;
        grid.cell_size = size;

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
                                column_border_intersection<GridRounding::Cell>(grid, a_x, a_y, b_x, b_y, c_x);
                            ASSERT_TRUE(check_column_border_intersecion(a_x, a_y, b_x, b_y, grid.cell_size, c_x, c_y))
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

} // namespace ka
