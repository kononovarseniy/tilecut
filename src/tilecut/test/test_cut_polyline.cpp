#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <tuple>
#include <vector>

#include <ka/common/fixed.hpp>
#include <ka/geometry_types/Vec2.hpp>
#include <ka/tilecut/cut_polyline.hpp>

#include "debug_output.hpp"

namespace ka
{

using ::testing::ElementsAreArray;

constexpr u16 g_tile_size = 100;

TEST(CutPolylineTest, empty)
{
    const TileGrid tile_grid { g_tile_size };

    std::vector<Vec2s64> line;

    cut_polyline(
        tile_grid,
        line,
        {},
        [](const auto &, auto, auto)
        {
            FAIL();
        });
}

TEST(CutPolylineTest, single_point)
{
    const TileGrid tile_grid { g_tile_size };

    std::vector<Vec2s64> line { { 5343, -9 } };

    size_t i = 0;
    cut_polyline(
        tile_grid,
        line,
        {},
        [&](const auto & tile, auto start, auto stop)
        {
            EXPECT_EQ(i++, 0);
            EXPECT_EQ(tile, Vec2s64(53, -1));
            EXPECT_EQ(start, line.begin());
            EXPECT_EQ(stop, line.end());
        });
    EXPECT_EQ(i, 1);
}

TEST(CutPolylineTest, all_same_xy)
{
    const TileGrid tile_grid { g_tile_size };

    std::vector<Vec2s64> line;
    line.assign(100, { -1000 + 34, -2000 + 5 });

    size_t i = 0;
    cut_polyline(
        tile_grid,
        line,
        {},
        [&](const auto & tile, auto start, auto stop)
        {
            EXPECT_EQ(i++, 0);
            EXPECT_EQ(tile, Vec2s64(-10, -20));
            EXPECT_EQ(start, line.begin());
            EXPECT_EQ(stop, line.end());
        });
    EXPECT_EQ(i, 1);
}

TEST(CutPolylineTest, all_same_xy_projected)
{
    struct Vertex final
    {
        Vec2s64 xy;
        f64 z;
    };

    const TileGrid tile_grid { g_tile_size };

    std::vector<Vertex> line;
    for (size_t i = 0; i < 100; ++i)
    {
        line.push_back({ { g_tile_size, g_tile_size }, exact_cast<f64>(i) });
    }

    size_t i = 0;
    cut_polyline(
        tile_grid,
        line,
        &Vertex::xy,
        [&](const auto & tile, auto start, auto stop)
        {
            EXPECT_EQ(i++, 0);
            EXPECT_EQ(tile, Vec2s64(1, 1));
            EXPECT_EQ(start, line.begin());
            EXPECT_EQ(stop, line.end());
        });
    EXPECT_EQ(i, 1);
}

TEST(CutPolylineTest, two_parts)
{
    const TileGrid tile_grid { g_tile_size };

    // clang-format off
    std::vector<Vec2s64> line {
        { 0, 0 },
        { g_tile_size, 0 },
        { g_tile_size, 0 },
        { g_tile_size, 0 },
        { g_tile_size, 0 },
        { g_tile_size, 50 },
        { g_tile_size, 50 },
        { g_tile_size + 1, 50 }
    };
    // clang-format on

    using It = decltype(line)::iterator;
    const auto it = line.begin();
    std::vector<std::tuple<Vec2s64, It, It>> expected {
        { { 0, 0 }, it + 0, it + 7 },
        { { 1, 0 }, it + 6, it + 8 },
    };

    size_t i = 0;
    cut_polyline(
        tile_grid,
        line,
        {},
        [&](const auto & tile, auto start, auto stop)
        {
            EXPECT_LT(i, expected.size());
            EXPECT_EQ(tile, std::get<0>(expected[i]));
            EXPECT_EQ(start, std::get<1>(expected[i]));
            EXPECT_EQ(stop, std::get<2>(expected[i]));
            ++i;
        });
    EXPECT_EQ(i, expected.size());
}

TEST(CutPolylineTest, intermediate_segment_almost_in_other_tile)
{
    const TileGrid tile_grid { g_tile_size };

    // clang-format off
    std::vector<Vec2s64> line {
        { 0, 0 },
        { g_tile_size, 50 },
        { g_tile_size, 10 },
        { 50, 0 },
    };
    // clang-format on

    size_t i = 0;
    cut_polyline(
        tile_grid,
        line,
        {},
        [&](const auto & tile, auto start, auto stop)
        {
            EXPECT_EQ(i++, 0);
            EXPECT_EQ(tile, Vec2s64(0, 0));
            EXPECT_EQ(start, line.begin());
            EXPECT_EQ(stop, line.end());
        });
    EXPECT_EQ(i, 1);
}

TEST(CutPolylineTest, degenerate_first_segments)
{
    const TileGrid tile_grid { g_tile_size };

    // clang-format off
    std::vector<Vec2s64> line {
        { 34, 9 },
        { 34, 9 },
        { 34, 9 },
        { 34, 9 },
        { 34, 9 },
        { 34, 9 },
        { g_tile_size, 50 },
        { g_tile_size, 10 },
        { 50, 0 },
    };
    // clang-format on

    size_t i = 0;
    cut_polyline(
        tile_grid,
        line,
        {},
        [&](const auto & tile, auto start, auto stop)
        {
            EXPECT_EQ(i++, 0);
            EXPECT_EQ(tile, Vec2s64(0, 0));
            EXPECT_EQ(start, line.begin());
            EXPECT_EQ(stop, line.end());
        });
    EXPECT_EQ(i, 1);
}

} // namespace ka
