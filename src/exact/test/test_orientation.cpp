#include <gtest/gtest.h>

#include <cmath>

#include <ka/exact/orientation.hpp>

namespace ka
{

TEST(OrientationTest, simple_collinear)
{
    EXPECT_EQ(orientation(1.0, 2.0, 6.0, 10.0, 11.0, 18.0), 0.0);
}

TEST(OrientationTest, simple_right_turn)
{
    EXPECT_LT(orientation(1.0, 2.0, 6.0, 10.0, 11.0 + 1.0, 18.0), 0.0);
}

TEST(OrientationTest, simple_left_turn)
{
    EXPECT_GT(orientation(1.0, 2.0, 6.0, 10.0, 11.0 - 1.0, 18.0), 0.0);
}

TEST(OrientationTest, hard_collinear)
{
    EXPECT_EQ(
        orientation(
            std::nextafter(1.0 + 1e6, std::numeric_limits<f64>::infinity()),
            2.0 + 1e6,
            std::nextafter(6.0 + 1e6, std::numeric_limits<f64>::infinity()),
            10.0 + 1e6,
            std::nextafter(11.0 + 1e6, std::numeric_limits<f64>::infinity()),
            18.0 + 1e6),
        0.0);
}

TEST(OrientationTest, slight_right_turn)
{
    EXPECT_LT(
        orientation(
            1.0 + 1e6,
            2.0 + 1e6,
            6.0 + 1e6,
            10.0 + 1e6,
            std::nextafter(11.0 + 1e6, std::numeric_limits<f64>::infinity()),
            18.0 + 1e6),
        0.0);
}

TEST(OrientationTest, slight_left_turn)
{
    EXPECT_GT(
        orientation(
            1.0 + 1e6,
            2.0 + 1e6,
            6.0 + 1e6,
            10.0 + 1e6,
            std::nextafter(11.0 + 1e6, -std::numeric_limits<f64>::infinity()),
            18.0 + 1e6),
        0.0);
}

} // namespace ka
