#include <gtest/gtest.h>

#include <ka/common/fixed.hpp>

#include "check_column_border_intersection.hpp"

namespace ka
{

TEST(CheckColumnBorderIntersectionTest, horizontal_line_zero_y)
{
    const f64 size = 0.3;
    const f64 a_x = -10, a_y = 0.0;
    const f64 b_x = 10, b_y = a_y;
    const f64 c_x = 14, c_y = 0;
    EXPECT_TRUE(check_column_border_intersecion(a_x, a_y, b_x, b_y, size, c_x, c_y));
    EXPECT_TRUE(check_column_border_intersecion(b_x, b_y, a_x, a_y, size, c_x, c_y));
}

TEST(CheckColumnBorderIntersectionTest, horizontal_line_positive_y)
{
    const f64 size = 0.3;
    const f64 a_x = -10, a_y = 0.1;
    const f64 b_x = 10, b_y = a_y;
    const f64 c_x = 14, c_y = 0;
    EXPECT_TRUE(check_column_border_intersecion(a_x, a_y, b_x, b_y, size, c_x, c_y));
    EXPECT_TRUE(check_column_border_intersecion(b_x, b_y, a_x, a_y, size, c_x, c_y));
}

TEST(CheckColumnBorderIntersectionTest, horizontal_line_bigger_positive_y)
{
    const f64 size = 0.3;
    const f64 a_x = -10, a_y = 0.3;
    const f64 b_x = 10, b_y = a_y;
    const f64 c_x = 14, c_y = 1;
    EXPECT_TRUE(check_column_border_intersecion(a_x, a_y, b_x, b_y, size, c_x, c_y));
    EXPECT_TRUE(check_column_border_intersecion(b_x, b_y, a_x, a_y, size, c_x, c_y));
}

TEST(CheckColumnBorderIntersectionTest, horizontal_line_even_bigger_positive_y)
{
    const f64 size = 0.3;
    const f64 a_x = -10, a_y = 0.30001;
    const f64 b_x = 10, b_y = a_y;
    const f64 c_x = 14, c_y = 1;
    EXPECT_TRUE(check_column_border_intersecion(a_x, a_y, b_x, b_y, size, c_x, c_y));
    EXPECT_TRUE(check_column_border_intersecion(b_x, b_y, a_x, a_y, size, c_x, c_y));
}

TEST(CheckColumnBorderIntersectionTest, horizontal_line_negative_y)
{
    const f64 size = 0.3;
    const f64 a_x = -10, a_y = -0.1;
    const f64 b_x = 10, b_y = a_y;
    const f64 c_x = 14, c_y = -1;
    EXPECT_TRUE(check_column_border_intersecion(a_x, a_y, b_x, b_y, size, c_x, c_y));
    EXPECT_TRUE(check_column_border_intersecion(b_x, b_y, a_x, a_y, size, c_x, c_y));
}

TEST(CheckColumnBorderIntersectionTest, simple_giagonal)
{
    const f64 size = 1;
    const f64 a_x = -5, a_y = 5;
    const f64 b_x = 5, b_y = -5;
    EXPECT_TRUE(check_column_border_intersecion(a_x, a_y, b_x, b_y, size, -5, 5)) << "First endpoint";
    EXPECT_TRUE(check_column_border_intersecion(a_x, a_y, b_x, b_y, size, -1, 1));
    EXPECT_TRUE(check_column_border_intersecion(a_x, a_y, b_x, b_y, size, 0, 0));
    EXPECT_TRUE(check_column_border_intersecion(a_x, a_y, b_x, b_y, size, 1, -1));
    EXPECT_TRUE(check_column_border_intersecion(a_x, a_y, b_x, b_y, size, 5, -5)) << "Second endpoint";

    EXPECT_TRUE(check_column_border_intersecion(b_x, b_y, a_x, a_y, size, -5, 5)) << "First endpoint opposite";
    EXPECT_TRUE(check_column_border_intersecion(b_x, b_y, a_x, a_y, size, -1, 1));
    EXPECT_TRUE(check_column_border_intersecion(b_x, b_y, a_x, a_y, size, 0, 0));
    EXPECT_TRUE(check_column_border_intersecion(b_x, b_y, a_x, a_y, size, 1, -1));
    EXPECT_TRUE(check_column_border_intersecion(b_x, b_y, a_x, a_y, size, 5, -5)) << "Second endpoint opposite";
}

TEST(CheckColumnBorderIntersectionTest, simple_line)
{
    const f64 size = 0.2;
    const f64 a_x = -2.4, a_y = 0.1;
    const f64 b_x = 1.2, b_y = -1.0;
    EXPECT_TRUE(check_column_border_intersecion(a_x, a_y, b_x, b_y, size, -12, 0)) << "First endpoint";
    EXPECT_TRUE(check_column_border_intersecion(a_x, a_y, b_x, b_y, size, -11, 0));
    EXPECT_TRUE(check_column_border_intersecion(a_x, a_y, b_x, b_y, size, -10, -1));
    EXPECT_TRUE(check_column_border_intersecion(a_x, a_y, b_x, b_y, size, -7, -2));
    EXPECT_TRUE(check_column_border_intersecion(a_x, a_y, b_x, b_y, size, -4, -2));
    EXPECT_TRUE(check_column_border_intersecion(a_x, a_y, b_x, b_y, size, 0, -4));
    EXPECT_TRUE(check_column_border_intersecion(a_x, a_y, b_x, b_y, size, 6, -5)) << "Second endpoint";

    EXPECT_TRUE(check_column_border_intersecion(b_x, b_y, a_x, a_y, size, -12, 0)) << "First endpoint opposite";
    EXPECT_TRUE(check_column_border_intersecion(b_x, b_y, a_x, a_y, size, -11, 0));
    EXPECT_TRUE(check_column_border_intersecion(b_x, b_y, a_x, a_y, size, -10, -1));
    EXPECT_TRUE(check_column_border_intersecion(b_x, b_y, a_x, a_y, size, -7, -2));
    EXPECT_TRUE(check_column_border_intersecion(b_x, b_y, a_x, a_y, size, -4, -2));
    EXPECT_TRUE(check_column_border_intersecion(b_x, b_y, a_x, a_y, size, 0, -4));
    EXPECT_TRUE(check_column_border_intersecion(b_x, b_y, a_x, a_y, size, 6, -5)) << "Second endpoint opposite";
}

} // namespace ka
