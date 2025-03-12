#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <algorithm>
#include <iterator>
#include <vector>

#include <ka/common/fixed.hpp>
#include <ka/exact/GridParameters.hpp>
#include <ka/exact/GridRounding.hpp>
#include <ka/geometry_types/Segment2.hpp>
#include <ka/geometry_types/Vec2.hpp>
#include <ka/tilecut/TileCellGrid.hpp>

#include "debug_output.hpp"
#include "mock_grid_parameters.hpp"

namespace ka
{

using ::testing::ElementsAreArray;

inline namespace
{

constexpr f64 g_cell_size = 1.0;
constexpr u16 g_tile_size = 100;

} // namespace

static_assert(TileCellGrid<GridRounding::NearestNode>::rounding == GridRounding::NearestNode);

TEST(TileCellGridTest, cell_of)
{
    const auto grid = make_grid<GridRounding::NearestNode>(g_cell_size, {}, g_tile_size);
    const Vec2f64 point { 3456.3546, 28.0 };
    const Vec2s64 expected { 3456, 28 };

    EXPECT_EQ(grid.cell_of(point), expected);
}

TEST(TileCellGridTest, tile_boundary_intersection_cells)
{
    const auto grid = make_grid<GridRounding::NearestNode>(g_cell_size, {}, g_tile_size);
    const Segment2f64 segment {
        { -0.49, 1 },
        { 2.49, 196 },
    };
    const Segment2s64 segment_cells { grid.cell_of(segment.a), grid.cell_of(segment.b) };
    const Segment2s64 expected_segment_cells { { 0, 1 }, { 2, 196 } };
    EXPECT_EQ(segment_cells, expected_segment_cells);

    std::vector<Vec2s64> result;
    grid.tile_boundary_intersection_cells(segment, segment_cells, std::back_inserter(result));

    const std::vector<Vec2s64> expected { { 0, 33 }, { 1, 100 } };
    std::ranges::sort(result);
    EXPECT_EQ(result, expected);
}

TEST(TileCellGridTest, tile_boundary_intersection_cells_x_centered_tile)
{
    const Vec2s64 origin { -g_tile_size / 2, 0 };
    const auto grid = make_grid<GridRounding::NearestNode>(g_cell_size, origin, g_tile_size);
    const Segment2f64 segment {
        { -51, -51 },
        { 51, 51 },
    };
    const Segment2s64 segment_cells { grid.cell_of(segment.a), grid.cell_of(segment.b) };
    const Segment2s64 expected_segment_cells { { -51, -51 }, { 51, 51 } };
    EXPECT_EQ(segment_cells, expected_segment_cells);

    std::vector<Vec2s64> result;
    grid.tile_boundary_intersection_cells(segment, segment_cells, std::back_inserter(result));

    const std::vector<Vec2s64> expected { { -50, -50 }, { 0, 0 }, { 50, 50 } };
    std::ranges::sort(result);
    EXPECT_EQ(result, expected);
}

TEST(TileCellGridTest, tile_boundary_intersection_cells_centered_tile)
{
    const Vec2s64 origin { -g_tile_size / 2, -g_tile_size / 2 };
    const auto grid = make_grid<GridRounding::NearestNode>(g_cell_size, origin, g_tile_size);
    const Segment2f64 segment {
        { -51, -51 },
        { 51, 51 },
    };
    const Segment2s64 segment_cells { grid.cell_of(segment.a), grid.cell_of(segment.b) };
    const Segment2s64 expected_segment_cells { { -51, -51 }, { 51, 51 } };
    EXPECT_EQ(segment_cells, expected_segment_cells);

    std::vector<Vec2s64> result;
    grid.tile_boundary_intersection_cells(segment, segment_cells, std::back_inserter(result));

    // Duplicate cells are normal for the above method.
    const std::vector<Vec2s64> expected { { -50, -50 }, { -50, -50 }, { 50, 50 }, { 50, 50 } };
    std::ranges::sort(result);
    EXPECT_EQ(result, expected);
}

TEST(TileCellGridTest, tile_boundary_intersection_cells_non_trivial_cell)
{
    const auto grid = make_grid<GridRounding::NearestNode>(5.0, {}, g_tile_size);
    const Segment2f64 segment {
        { 550, 503 },
        { 555, 497 },
    };
    const Segment2s64 segment_cells { grid.cell_of(segment.a), grid.cell_of(segment.b) };
    const Segment2s64 expected_segment_cells { { 110, 101 }, { 111, 99 } };
    EXPECT_EQ(segment_cells, expected_segment_cells);

    std::vector<Vec2s64> result;
    grid.tile_boundary_intersection_cells(segment, segment_cells, std::back_inserter(result));

    const std::vector<Vec2s64> expected { { 111, 100 } };
    std::ranges::sort(result);
    EXPECT_EQ(result, expected);
}

TEST(TileCellGridTest, tile_boundary_intersection_cells_same_cell)
{
    const auto grid = make_grid<GridRounding::NearestNode>(5.0, {}, g_tile_size);
    const Segment2f64 segment {
        { 550, 501 },
        { 551, 499 },
    };
    const Segment2s64 segment_cells { grid.cell_of(segment.a), grid.cell_of(segment.b) };
    const Segment2s64 expected_segment_cells { { 110, 100 }, { 110, 100 } };
    EXPECT_EQ(segment_cells, expected_segment_cells);

    std::vector<Vec2s64> result;
    grid.tile_boundary_intersection_cells(segment, segment_cells, std::back_inserter(result));

    const std::vector<Vec2s64> expected { { 110, 100 } };
    std::ranges::sort(result);
    EXPECT_EQ(result, expected);
}

} // namespace ka
