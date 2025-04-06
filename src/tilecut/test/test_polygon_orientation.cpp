#include <gtest/gtest.h>

#include <cmath>
#include <ranges>
#include <vector>

#include <ka/geometry_types/Segment2.hpp>
#include <ka/geometry_types/Vec2.hpp>
#include <ka/tilecut/polygon_orientation.hpp>

namespace ka
{

inline namespace
{

template <typename T>
[[nodiscard]] std::vector<Segment2<T>> to_segments(const std::vector<Vec2<T>> & contour)
{
    std::vector<Segment2<T>> result;
    result.reserve(contour.size() - 1);
    auto prev = contour.front();
    for (const auto & curr : std::ranges::drop_view { contour, 1 })
    {
        result.push_back({ prev, curr });
        prev = curr;
    }
    return result;
}

} // namespace

TEST(PolygonOrientationTest, simple_triangle)
{
    std::vector<Vec2f64> contour {
        { 0, 0 },
        { 1, 0 },
        { 0, 1 },
        { 0, 0 },
    };
    EXPECT_EQ(contour_orientation(contour), PolygonOrientation::CounterClockwise);
    EXPECT_EQ(polygon_orientation(to_segments(contour)), PolygonOrientation::CounterClockwise);
    std::ranges::reverse(contour);
    EXPECT_EQ(contour_orientation(contour), PolygonOrientation::Clockwise);
    EXPECT_EQ(polygon_orientation(to_segments(contour)), PolygonOrientation::Clockwise);
}

TEST(PolygonOrientationTest, simple_triangle_two_right_vertices)
{
    std::vector<Vec2f64> contour {
        { 0, 0 },
        { 1, 0 },
        { 1, 1 },
        { 0, 0 },
    };
    EXPECT_EQ(contour_orientation(contour), PolygonOrientation::CounterClockwise);
    EXPECT_EQ(polygon_orientation(to_segments(contour)), PolygonOrientation::CounterClockwise);
    std::ranges::reverse(contour);
    EXPECT_EQ(contour_orientation(contour), PolygonOrientation::Clockwise);
    EXPECT_EQ(polygon_orientation(to_segments(contour)), PolygonOrientation::Clockwise);
}

TEST(PolygonOrientationTest, simple_triangle_almost_two_right_vertices)
{
    std::vector<Vec2f64> contour {
        { 0, 0 },
        { std::nextafter(1, -std::numeric_limits<f64>::infinity()), 0 },
        { 1, 1 },
        { 0, 0 },
    };
    EXPECT_EQ(contour_orientation(contour), PolygonOrientation::CounterClockwise);
    EXPECT_EQ(polygon_orientation(to_segments(contour)), PolygonOrientation::CounterClockwise);
    std::ranges::reverse(contour);
    EXPECT_EQ(contour_orientation(contour), PolygonOrientation::Clockwise);
    EXPECT_EQ(polygon_orientation(to_segments(contour)), PolygonOrientation::Clockwise);
}

TEST(PolygonOrientationTest, multiple_right_vertices)
{
    std::vector<Vec2f64> contour {
        { -100, -10 }, { 1, -100 }, { 1, -99 }, { 1, -98 }, { 1, -90 }, { -100, -10 },
    };
    EXPECT_EQ(contour_orientation(contour), PolygonOrientation::CounterClockwise);
    EXPECT_EQ(polygon_orientation(to_segments(contour)), PolygonOrientation::CounterClockwise);
    std::ranges::reverse(contour);
    EXPECT_EQ(contour_orientation(contour), PolygonOrientation::Clockwise);
    EXPECT_EQ(polygon_orientation(to_segments(contour)), PolygonOrientation::Clockwise);
}

TEST(PolygonOrientationTest, multiple_touchpoints)
{
    std::vector<Vec2f64> contour {
        { -1, 10 },
        { 0, 10 },
    };
    f64 current_y = 10;
    for (int i = 0; i < 5; ++i)
    {
        contour.push_back({ 1e9, -10 });
        current_y = std::nextafter(current_y, -std::numeric_limits<f64>::infinity());
        contour.push_back({ 0, current_y });
        current_y = std::nextafter(current_y, -std::numeric_limits<f64>::infinity());
        contour.push_back({ 0, current_y });
    }
    contour.push_back(contour.front());

    EXPECT_EQ(contour_orientation(contour), PolygonOrientation::Clockwise);
    EXPECT_EQ(polygon_orientation(to_segments(contour)), PolygonOrientation::Clockwise);
    std::ranges::reverse(contour);
    EXPECT_EQ(contour_orientation(contour), PolygonOrientation::CounterClockwise);
    EXPECT_EQ(polygon_orientation(to_segments(contour)), PolygonOrientation::CounterClockwise);
}

} // namespace ka
