#include <gtest/gtest.h>

#include <ka/common/cast.hpp>
#include <ka/geometry_types/Vec2.hpp>
#include <ka/tilecut/lerp_along_segment.hpp>

namespace ka
{

TEST(LerpAlongSegmentTest, horizontal_test)
{
    const Vec2f64 start_pos { 1000.0, 1000.0 };
    const f64 start_z = 1.0;
    const Vec2f64 stop_pos { 1010.0, 1000.0 };
    const f64 stop_z = 11.0;

    for (int i = 0; i < 10; ++i)
    {
        const Vec2f64 pos { start_pos.x + safe_cast<f64>(i), start_pos.y };
        const auto result = lerp_along_segment(start_pos, start_z, stop_pos, stop_z, pos);
        const auto expected = 1.0 + safe_cast<f64>(i);
        EXPECT_EQ(result, expected);
    }
}

TEST(LerpAlongSegmentTest, orthogonal_test)
{
    const Vec2f64 start_pos { 1000.0, 1000.0 };
    const f64 start_z = 1.0;
    const Vec2f64 stop_pos { 1010.0, 1000.0 };
    const f64 stop_z = 11.0;

    for (int i = 0; i < 10; ++i)
    {
        const Vec2f64 pos { start_pos.x, start_pos.y + safe_cast<f64>(i) };
        const auto result = lerp_along_segment(start_pos, start_z, stop_pos, stop_z, pos);
        const auto expected = 1.0;
        EXPECT_EQ(result, expected);
    }
}

TEST(LerpAlongSegmentTest, diagonal_test)
{
    const Vec2f64 start_pos { 1000.0, 1000.0 };
    const f64 start_z = 1.0;
    const Vec2f64 stop_pos { 1010.0, 1010.0 };
    const f64 stop_z = 11.0;

    for (int i = 0; i < 10; ++i)
    {
        const Vec2f64 pos { start_pos.x + safe_cast<f64>(i), start_pos.y + safe_cast<f64>(i) };
        const auto result = lerp_along_segment(start_pos, start_z, stop_pos, stop_z, pos);
        const auto expected = 1.0 + safe_cast<f64>(i);
        EXPECT_EQ(result, expected);
    }
}

TEST(LerpAlongSegmentTest, side_test)
{
    const Vec2f64 start_pos { 1000.0, 1000.0 };
    const f64 start_z = 1.0;
    const Vec2f64 stop_pos { 1010.0, 1010.0 };
    const f64 stop_z = 11.0;

    for (int i = 0; i < 10; ++i)
    {
        const Vec2f64 pos { start_pos.x, start_pos.y + safe_cast<f64>(i) };
        const auto result = lerp_along_segment(start_pos, start_z, stop_pos, stop_z, pos);
        const auto expected = 1.0 + safe_cast<f64>(i) * 0.5;
        EXPECT_EQ(result, expected);
    }
}

} // namespace ka
