#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <array>
#include <cmath>
#include <concepts>
#include <format>
#include <iterator>
#include <vector>

#include <ka/exact/HotPixelCollector.hpp>
#include <ka/exact/snap_round.hpp>
#include <ka/geometry_types/Vec2.hpp>

namespace ka
{

using ::testing::ElementsAreArray;

template <typename T>
std::ostream & operator<<(std::ostream & stream, const Vec2<T> & point)
{
    stream << std::format("({}, {})", point.x, point.y);
    return stream;
}

[[nodiscard]] auto next_float(const std::floating_point auto value)
{
    return std::nextafter(value, std::numeric_limits<decltype(value)>::infinity());
}

[[nodiscard]] auto prev_float(const std::floating_point auto value)
{
    return std::nextafter(value, -std::numeric_limits<decltype(value)>::infinity());
}

TEST(SnapRoundingTest, distorted_square)
{
    constexpr auto gird_step = 1.1;
    constexpr auto tile_step = 8;

    const std::array<Vec2f64, 5> geometry { {
        { prev_float(gird_step * -4), gird_step * -4 },
        { next_float(gird_step * +4), prev_float(gird_step * -4) },
        { prev_float(gird_step * +4), next_float(gird_step * +4) },
        { gird_step * -4, gird_step * +4 },
        { prev_float(gird_step * -4), gird_step * -4 },
    } };

    // clang-format off
    const std::vector<Vec2s64> expected {
        { -5, -4 },
        {  0, -5 },
        { +4, -5 },
        { +4,  0 }, // Exactly at corner.
        { +3, +4 },
        {  0, +4 },
        { -4, +4 },
        { -5,  0 },
        { -5, -4 },
    };
    // clang-format on

    HotPixelCollector collector;
    collector.init(gird_step, tile_step);
    collector.new_contour();
    for (const auto & vertex : geometry)
    {
        collector.add_vertex_and_tile_cuts<GridRounding::Cell>(vertex);
    }
    const auto & hot_pixels = collector.build_index();

    std::vector<Vec2s64> result;
    snap_round<GridRounding::Cell>(hot_pixels, geometry, std::back_inserter(result));

    EXPECT_THAT(result, ElementsAreArray(expected));
}

TEST(SnapRoundingTest, perfect_square)
{
    constexpr auto gird_step = 1.1;
    constexpr auto tile_step = 8;

    const std::array<Vec2f64, 5> geometry { {
        { gird_step * -4, gird_step * -4 },
        { gird_step * +4, gird_step * -4 },
        { gird_step * +4, gird_step * +4 },
        { gird_step * -4, gird_step * +4 },
        { gird_step * -4, gird_step * -4 },
    } };

    // clang-format off
    const std::vector<Vec2s64> expected {
        { -4, -4 },
        {  0, -4 },
        { +4, -4 },
        { +4,  0 },
        { +4, +4 },
        {  0, +4 },
        { -4, +4 },
        { -4,  0 },
        { -4, -4 },
    };
    // clang-format on

    HotPixelCollector collector;
    collector.init(gird_step, tile_step);
    collector.new_contour();
    for (const auto & vertex : geometry)
    {
        collector.add_vertex_and_tile_cuts<GridRounding::Cell>(vertex);
    }
    const auto & hot_pixels = collector.build_index();

    std::vector<Vec2s64> result;
    snap_round<GridRounding::Cell>(hot_pixels, geometry, std::back_inserter(result));

    EXPECT_THAT(result, ElementsAreArray(expected));
}

TEST(SnapRoundingTest, half_integer_perfect_square)
{
    constexpr auto gird_step = 1;
    constexpr auto tile_step = 8;

    // clang-format off
    const std::array<Vec2f64, 5> geometry { {
        { -4.0 + 0.5, -4.0 + 0.5 },
        { +4.0 + 0.5, -4.0 + 0.5 },
        { +4.0 + 0.5, +4.0 + 0.5 },
        { -4.0 + 0.5, +4.0 + 0.5 },
        { -4.0 + 0.5, -4.0 + 0.5 },
    } };

    const std::vector<Vec2s64> expected {
        { -4, -4 },
        {  0, -4 },
        { +4, -4 },
        { +4,  0 },
        { +4, +4 },
        {  0, +4 },
        { -4, +4 },
        { -4,  0 },
        { -4, -4 },
    };
    // clang-format on

    HotPixelCollector collector;
    collector.init(gird_step, tile_step);
    collector.new_contour();
    for (const auto & vertex : geometry)
    {
        collector.add_vertex_and_tile_cuts<GridRounding::Cell>(vertex);
    }
    const auto & hot_pixels = collector.build_index();

    std::vector<Vec2s64> result;
    snap_round<GridRounding::Cell>(hot_pixels, geometry, std::back_inserter(result));

    EXPECT_THAT(result, ElementsAreArray(expected));
}

} // namespace ka
