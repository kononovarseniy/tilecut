#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <iterator>

#include <ka/common/fixed.hpp>
#include <ka/geometry_types/Segment2.hpp>
#include <ka/geometry_types/Vec2.hpp>
#include <ka/tilecut/TileGrid.hpp>

#include "debug_output.hpp"

namespace ka
{

constexpr u16 g_tile_size = 100;

TEST(TileGridTest, tile_of_point)
{
    const TileGrid tile_grid { g_tile_size };
    const Vec2s64 cell { 40, 50 };
    const Vec2s64 expected_tile { 0, 0 };
    EXPECT_EQ(tile_grid.tile_of(cell), expected_tile);
}

TEST(TileGridTest, tile_of_point_big)
{
    const TileGrid tile_grid { g_tile_size };
    const Vec2s64 cell { 3000, 10001 };
    const Vec2s64 expected_tile { 30, 100 };
    EXPECT_EQ(tile_grid.tile_of(cell), expected_tile);
}

TEST(TileGridTest, tile_of_point_negative)
{
    const TileGrid tile_grid { g_tile_size };
    const Vec2s64 cell { -3000, -10001 };
    const Vec2s64 expected_tile { -30, -101 };
    EXPECT_EQ(tile_grid.tile_of(cell), expected_tile);
}

TEST(TileGridTest, local_coordinates)
{
    const TileGrid tile_grid { g_tile_size };
    const Segment2s64 segment {
        { -201, -110 },
        { -200, -190 },
    };
    const Vec2s64 tile { -3, -2 };
    const Segment2u16 expected { { 99, 90 }, { 100, 10 } };
    EXPECT_EQ(tile_grid.local_coordinates(tile, segment), expected);
}

TEST(TileGridTest, is_inside_single_tile)
{
    const TileGrid tile_grid { g_tile_size };
    const Segment2s64 segment {
        { 0, 0 },
        { 50, 50 },
    };
    EXPECT_TRUE(tile_grid.is_inside_single_tile(segment));
}

TEST(TileGridTest, is_inside_single_tile_false)
{
    const TileGrid tile_grid { g_tile_size };
    const Segment2s64 segment {
        { 99, 49 },
        { 101, 50 },
    };
    EXPECT_FALSE(tile_grid.is_inside_single_tile(segment));
}

TEST(TileGridTest, is_inside_single_tile_diagonal)
{
    const TileGrid tile_grid { g_tile_size };
    const Segment2s64 segment {
        { 0, 0 },
        { 100, 100 },
    };
    EXPECT_TRUE(tile_grid.is_inside_single_tile(segment));
}

TEST(TileGridTest, is_inside_single_tile_on_boundary)
{
    const TileGrid tile_grid { g_tile_size };
    const Segment2s64 segment {
        { -200, -110 },
        { -200, -190 },
    };
    EXPECT_TRUE(tile_grid.is_inside_single_tile(segment));
}

TEST(TileGridTest, is_inside_single_tile_on_boundary_outside)
{
    const TileGrid tile_grid { g_tile_size };
    const Segment2s64 segment {
        { -200, -101 },
        { -200, -99 },
    };
    EXPECT_FALSE(tile_grid.is_inside_single_tile(segment));
}

TEST(TileGridTest, is_inside_single_tile_on_boundary_short_inside)
{
    const TileGrid tile_grid { g_tile_size };
    const Segment2s64 segment {
        { -200, -100 },
        { -200, -99 },
    };
    EXPECT_TRUE(tile_grid.is_inside_single_tile(segment));
}

TEST(TileGridTest, tile_of_segment)
{
    const TileGrid tile_grid { g_tile_size };
    const Segment2s64 segment {
        { 0, 0 },
        { 50, 50 },
    };
    const Vec2s64 expected { 0, 0 };
    EXPECT_EQ(tile_grid.tile_of(segment), expected);
}

TEST(TileGridTest, tile_of_segment_diagonal)
{
    const TileGrid tile_grid { g_tile_size };
    const Segment2s64 segment {
        { 0, 0 },
        { 100, 100 },
    };
    const Vec2s64 expected { 0, 0 };
    EXPECT_EQ(tile_grid.tile_of(segment), expected);
}

TEST(TileGridTest, tile_of_segment_on_boundary)
{
    const TileGrid tile_grid { g_tile_size };
    {
        const Segment2s64 segment {
            { -200, -110 },
            { -200, -190 },
        };
        const Vec2s64 expected { -2, -2 };
        EXPECT_EQ(tile_grid.tile_of(segment), expected);
    }
    {
        const Segment2s64 segment {
            { -200, -190 },
            { -200, -110 },
        };
        const Vec2s64 expected { -3, -2 };
        EXPECT_EQ(tile_grid.tile_of(segment), expected);
    }
}

TEST(TileGridTest, tile_of_segment_on_boundary_short_inside)
{
    const TileGrid tile_grid { g_tile_size };
    {
        const Segment2s64 segment {
            { -200, -100 },
            { -200, -99 },
        };
        const Vec2s64 expected { -3, -1 };
        EXPECT_EQ(tile_grid.tile_of(segment), expected);
    }
    {
        const Segment2s64 segment {
            { -200, -99 },
            { -200, -100 },
        };
        const Vec2s64 expected { -2, -1 };
        EXPECT_EQ(tile_grid.tile_of(segment), expected);
    }
}

TEST(TileGridTest, intersected_boundaries_ranges)
{
    const TileGrid tile_grid { g_tile_size };
    const Segment2s64 segment {
        { -200, -101 },
        { -200, -99 },
    };
    const auto [min_x, max_x, min_y, max_y] = tile_grid.intersected_boundaries_ranges(segment);
    EXPECT_EQ(min_x, -200);
    EXPECT_EQ(max_x, -200);
    EXPECT_EQ(min_y, -100);
    EXPECT_EQ(max_y, -100);
}

TEST(TileGridTest, intersected_boundaries_ranges_no_y)
{
    const TileGrid tile_grid { g_tile_size };
    const Segment2s64 segment {
        { 99, 49 },
        { 101, 50 },
    };
    const auto [min_x, max_x, min_y, max_y] = tile_grid.intersected_boundaries_ranges(segment);
    EXPECT_EQ(min_x, 100);
    EXPECT_EQ(max_x, 100);
    EXPECT_EQ(min_y, 100);
    EXPECT_EQ(max_y, 0);
}

TEST(TileGridTest, intersected_boundaries_ranges_nagative_values)
{
    const TileGrid tile_grid { g_tile_size };
    const Segment2s64 segment {
        { -3001, -2000 },
        { -1001, 10001 },
    };
    const auto [min_x, max_x, min_y, max_y] = tile_grid.intersected_boundaries_ranges(segment);
    EXPECT_EQ(min_x, -3000);
    EXPECT_EQ(max_x, -1100);
    EXPECT_EQ(min_y, -2000);
    EXPECT_EQ(max_y, 10000);
}

TEST(TileGridTest, tile_local_boundaries)
{
    const TileGrid tile_grid { g_tile_size };

    std::vector<Segment2u16> result;
    result.reserve(4);
    tile_grid.tile_local_boundaries(std::back_inserter(result));

    std::vector<Segment2u16> expected {
        { { 0, 0 }, { g_tile_size, 0 } },
        { { g_tile_size, 0 }, { g_tile_size, g_tile_size } },
        { { g_tile_size, g_tile_size }, { 0, g_tile_size } },
        { { 0, g_tile_size }, { 0, 0 } },
    };

    EXPECT_EQ(result, expected);
}

} // namespace ka
