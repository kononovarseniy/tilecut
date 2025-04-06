#include <gtest/gtest.h>

#include <cmath>

#include <ka/exact/orientation.hpp>

namespace ka
{

TEST(OrientationTest, simple_collinear)
{
    EXPECT_EQ(orientation(f32 { 1 }, f32 { 2 }, f32 { 6 }, f32 { 10 }, f32 { 11 }, f32 { 18 }), f32 { 0 });
    EXPECT_EQ(orientation(f64 { 1 }, f64 { 2 }, f64 { 6 }, f64 { 10 }, f64 { 11 }, f64 { 18 }), f64 { 0 });
    EXPECT_EQ(orientation(u16 { 1 }, u16 { 2 }, u16 { 6 }, u16 { 10 }, u16 { 11 }, u16 { 18 }), s64 { 0 });
    EXPECT_EQ(orientation(s16 { 1 }, s16 { 2 }, s16 { 6 }, s16 { 10 }, s16 { 11 }, s16 { 18 }), s64 { 0 });
}

TEST(OrientationTest, simple_negative)
{
    EXPECT_GT(orientation(f32 { 1 }, f32 { -98 }, f32 { 1 }, f32 { -90 }, f32 { -100 }, f32 { -10 }), f32 { 0 });
    EXPECT_GT(orientation(f64 { 1 }, f64 { -98 }, f64 { 1 }, f64 { -90 }, f64 { -100 }, f64 { -10 }), f64 { 0 });
    EXPECT_GT(orientation(s16 { 1 }, s16 { -98 }, s16 { 1 }, s16 { -90 }, s16 { -100 }, s16 { -10 }), s64 { 0 });
}

TEST(OrientationTest, simple_right_turn)
{
    EXPECT_LT(orientation(f32 { 1 }, f32 { 2 }, f32 { 6 }, f32 { 10 }, f32 { 11 + 1 }, f32 { 18 }), f32 { 0 });
    EXPECT_LT(orientation(f64 { 1 }, f64 { 2 }, f64 { 6 }, f64 { 10 }, f64 { 11 + 1 }, f64 { 18 }), f64 { 0 });
    EXPECT_LT(orientation(u16 { 1 }, u16 { 2 }, u16 { 6 }, u16 { 10 }, u16 { 11 + 1 }, u16 { 18 }), s64 { 0 });
    EXPECT_LT(orientation(s16 { 1 }, s16 { 2 }, s16 { 6 }, s16 { 10 }, s16 { 11 + 1 }, s16 { 18 }), s64 { 0 });
}

TEST(OrientationTest, simple_left_turn)
{
    EXPECT_GT(orientation(f32 { 1 }, f32 { 2 }, f32 { 6 }, f32 { 10 }, f32 { 11 - 1 }, f32 { 18 }), f32 { 0 });
    EXPECT_GT(orientation(f64 { 1 }, f64 { 2 }, f64 { 6 }, f64 { 10 }, f64 { 11 - 1 }, f64 { 18 }), f64 { 0 });
    EXPECT_GT(orientation(u16 { 1 }, u16 { 2 }, u16 { 6 }, u16 { 10 }, u16 { 11 - 1 }, u16 { 18 }), s64 { 0 });
    EXPECT_GT(orientation(s16 { 1 }, s16 { 2 }, s16 { 6 }, s16 { 10 }, s16 { 11 - 1 }, s16 { 18 }), s64 { 0 });
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
