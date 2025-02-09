#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <array>
#include <cmath>
#include <concepts>
#include <iterator>
#include <vector>

#include <ka/geometry_types/Vec2.hpp>
#include <ka/tilecut/HotPixelCollector.hpp>
#include <ka/tilecut/TileCellGrid.hpp>
#include <ka/tilecut/snap_round.hpp>

#include "debug_output.hpp"
#include "mock_grid_parameters.hpp"

namespace ka
{

using ::testing::ElementsAreArray;

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
    const auto grid = make_grid<GridRounding::Cell>(1.1, 8);

    const std::array<Vec2f64, 5> geometry { {
        { prev_float(grid.cell_size() * -4), grid.cell_size() * -4 },
        { next_float(grid.cell_size() * +4), prev_float(grid.cell_size() * -4) },
        { prev_float(grid.cell_size() * +4), next_float(grid.cell_size() * +4) },
        { grid.cell_size() * -4, grid.cell_size() * +4 },
        { prev_float(grid.cell_size() * -4), grid.cell_size() * -4 },
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
    collector.add_tile_snapped_polyline(grid, geometry);
    const auto & hot_pixels = collector.build_index();

    std::vector<Vec2s64> result;
    snap_round(grid, hot_pixels, geometry, std::back_inserter(result));

    EXPECT_THAT(result, ElementsAreArray(expected));
}

TEST(SnapRoundingTest, perfect_square)
{
    const auto grid = make_grid<GridRounding::Cell>(1.1, 8);

    const std::array<Vec2f64, 5> geometry { {
        { grid.cell_size() * -4, grid.cell_size() * -4 },
        { grid.cell_size() * +4, grid.cell_size() * -4 },
        { grid.cell_size() * +4, grid.cell_size() * +4 },
        { grid.cell_size() * -4, grid.cell_size() * +4 },
        { grid.cell_size() * -4, grid.cell_size() * -4 },
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
    collector.add_tile_snapped_polyline(grid, geometry);
    const auto & hot_pixels = collector.build_index();

    std::vector<Vec2s64> result;
    snap_round(grid, hot_pixels, geometry, std::back_inserter(result));

    EXPECT_THAT(result, ElementsAreArray(expected));
}

TEST(SnapRoundingTest, reusable_collector)
{
    HotPixelCollector collector;
    const auto grid = make_grid<GridRounding::Cell>(1.1, 8);

    {
        const std::array<Vec2f64, 5> geometry { {
            { prev_float(grid.cell_size() * -4), grid.cell_size() * -4 },
            { next_float(grid.cell_size() * +4), prev_float(grid.cell_size() * -4) },
            { prev_float(grid.cell_size() * +4), next_float(grid.cell_size() * +4) },
            { grid.cell_size() * -4, grid.cell_size() * +4 },
            { prev_float(grid.cell_size() * -4), grid.cell_size() * -4 },
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
        collector.add_tile_snapped_polyline(grid, geometry);
        const auto & hot_pixels = collector.build_index();

        std::vector<Vec2s64> result;
        snap_round(grid, hot_pixels, geometry, std::back_inserter(result));

        EXPECT_THAT(result, ElementsAreArray(expected));
    }

    collector.reset();

    {
        const std::array<Vec2f64, 5> geometry { {
            { grid.cell_size() * -4, grid.cell_size() * -4 },
            { grid.cell_size() * +4, grid.cell_size() * -4 },
            { grid.cell_size() * +4, grid.cell_size() * +4 },
            { grid.cell_size() * -4, grid.cell_size() * +4 },
            { grid.cell_size() * -4, grid.cell_size() * -4 },
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

        collector.add_tile_snapped_polyline(grid, geometry);
        const auto & hot_pixels = collector.build_index();

        std::vector<Vec2s64> result;
        snap_round(grid, hot_pixels, geometry, std::back_inserter(result));

        EXPECT_THAT(result, ElementsAreArray(expected));
    }
}

TEST(SnapRoundingTest, half_integer_perfect_square)
{
    const auto grid = make_grid<GridRounding::Cell>(1, 8);

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
    collector.add_tile_snapped_polyline(grid, geometry);
    const auto & hot_pixels = collector.build_index();

    std::vector<Vec2s64> result;
    snap_round(grid, hot_pixels, geometry, std::back_inserter(result));

    EXPECT_THAT(result, ElementsAreArray(expected));
}

} // namespace ka
