#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <iterator>
#include <limits>
#include <ranges>
#include <vector>

#include <ka/common/fixed.hpp>
#include <ka/geometry_types/Segment2.hpp>
#include <ka/geometry_types/Vec2.hpp>
#include <ka/tilecut/TileGrid.hpp>
#include <ka/tilecut/find_cuts.hpp>

#include "debug_output.hpp"

namespace ka
{

using ::testing::ElementsAreArray;

constexpr u16 g_max_tile_size = std::numeric_limits<u16>::max();
constexpr u16 g_tile_size = 100;

[[nodiscard]] std::vector<Segment2u16> make_line(const std::vector<Vec2u16> & points)
{
    std::vector<Segment2u16> result;
    result.reserve(points.size());
    auto prev = points.front();
    for (const auto curr : points | std::views::drop(1))
    {
        result.push_back({ prev, curr });
        prev = curr;
    }
    return result;
}

[[nodiscard]] std::vector<Segment2u16> all_cuts(const u16 tile_size)
{
    std::vector<Segment2u16> res;
    res.reserve(4);
    TileGrid { tile_size }.tile_local_boundaries(std::back_inserter(res));
    return res;
}

TEST(FindCutsTest, empty_input)
{
    const std::vector<Segment2u16> segments;
    const std::vector<Segment2u16> expected;

    std::vector<Segment2u16> result;
    find_cuts(segments, result, g_tile_size);
    EXPECT_EQ(result, expected);
}

TEST(FindCutsTest, full_tile_no_cuts)
{
    const auto segments = all_cuts(g_max_tile_size);
    const std::vector<Segment2u16> expected;

    std::vector<Segment2u16> result;
    find_cuts(segments, result, g_max_tile_size);
    EXPECT_EQ(result, expected);
}

TEST(FindCutsTest, square_no_cuts)
{
    // Shuffled square.
    const std::vector<Segment2u16> segments {
        { { 51, 50 }, { 51, 51 } },
        { { 50, 50 }, { 51, 50 } },
        { { 50, 51 }, { 50, 50 } },
        { { 51, 51 }, { 50, 51 } },
    };
    const std::vector<Segment2u16> expected;

    std::vector<Segment2u16> result;
    find_cuts(segments, result, g_tile_size);
    EXPECT_EQ(result, expected);
}

TEST(FindCutsTest, square_all_cuts)
{
    // Shuffled inverted square.
    const std::vector<Segment2u16> segments {
        { { 51, 51 }, { 51, 50 } },
        { { 51, 50 }, { 50, 50 } },
        { { 50, 50 }, { 50, 51 } },
        { { 50, 51 }, { 51, 51 } },
    };
    const auto expected = all_cuts(g_tile_size);

    std::vector<Segment2u16> result;
    find_cuts(segments, result, g_tile_size);
    EXPECT_EQ(result, expected);
}

TEST(FindCutsTest, difficult_no_cuts)
{
    const std::vector<Segment2u16> segments = make_line({
        { 50, 50 },
        { 51, 49 },
        { 90, 50 },
        { 80, 51 },
        { 90, 52 },
        { 90, 53 },
        { 50, 50 },
    });
    const std::vector<Segment2u16> expected;

    std::vector<Segment2u16> result;
    find_cuts(segments, result, g_tile_size);
    EXPECT_EQ(result, expected);
}

TEST(FindCutsTest, difficult_all_cuts)
{
    const std::vector<Segment2u16> segments = make_line({
        { 50, 50 },
        { 90, 53 },
        { 90, 52 },
        { 80, 51 },
        { 90, 50 },
        { 51, 49 },
        { 50, 50 },
    });
    const auto expected = all_cuts(g_tile_size);

    std::vector<Segment2u16> result;
    find_cuts(segments, result, g_tile_size);
    EXPECT_EQ(result, expected);
}

TEST(FindCutsTest, left_half)
{
    const auto tile_size = g_max_tile_size;

    const std::vector<Segment2u16> segments {
        { { tile_size / 2, 0 }, { tile_size / 2, tile_size } },
    };
    const auto expected = make_line({
        { tile_size / 2, tile_size },
        { 0, tile_size },
        { 0, 0 },
        { tile_size / 2, 0 },
    });

    std::vector<Segment2u16> result;
    find_cuts(segments, result, tile_size);
    EXPECT_EQ(result, expected);
}

TEST(FindCutsTest, small_corner)
{
    const auto tile_size = g_max_tile_size;

    const std::vector<Segment2u16> segments {
        { { 1, 0 }, { 0, 1 } },
    };
    const auto expected = make_line({
        { 0, 1 },
        { 0, 0 },
        { 1, 0 },
    });

    std::vector<Segment2u16> result;
    find_cuts(segments, result, tile_size);
    EXPECT_EQ(result, expected);
}

TEST(FindCutsTest, except_small_corner)
{
    const auto tile_size = g_max_tile_size;

    const std::vector<Segment2u16> segments {
        { { 0, 1 }, { 1, 0 } },
    };
    const auto expected = make_line({
        { 1, 0 },
        { tile_size, 0 },
        { tile_size, tile_size },
        { 0, tile_size },
        { 0, 1 },
    });

    std::vector<Segment2u16> result;
    find_cuts(segments, result, tile_size);
    EXPECT_EQ(result, expected);
}

TEST(FindCutsTest, two_cuts_on_with_segment_on_boundary)
{
    const auto tile_size = g_max_tile_size;

    std::vector<Segment2u16> segments {
        { { tile_size, 50 }, { tile_size, 55 } },
    };
    std::ranges::copy(
        make_line({
            { tile_size, 90 },
            { 50, 50 },
            { tile_size, 30 },
        }),
        std::back_inserter(segments));
    const std::vector<Segment2u16> expected {
        { { tile_size, 30 }, { tile_size, 50 } },
        { { tile_size, 55 }, { tile_size, 90 } },
    };

    std::vector<Segment2u16> result;
    find_cuts(segments, result, tile_size);
    EXPECT_EQ(result, expected);
}

TEST(FindCutsTest, triangle_vertex_on_the_right_all_cuts)
{
    const auto tile_size = g_max_tile_size;

    const auto segments = make_line({
        { tile_size, 50 },
        { tile_size - 10, 50 },
        { tile_size - 10, 40 },
        { tile_size, 50 },
    });
    const std::vector<Segment2u16> expected;

    std::vector<Segment2u16> result;
    find_cuts(segments, result, tile_size);
    EXPECT_EQ(result, expected);
}

TEST(FindCutsTest, inverted_triangle_vertex_on_the_right_all_cuts)
{
    const auto tile_size = g_max_tile_size;

    const auto segments = make_line({
        { tile_size, 50 },
        { tile_size - 10, 40 },
        { tile_size - 10, 50 },
        { tile_size, 50 },
    });
    const auto expected = make_line({
        { tile_size, 50 },
        { tile_size, tile_size },
        { 0, tile_size },
        { 0, 0 },
        { tile_size, 0 },
        { tile_size, 50 },
    });

    std::vector<Segment2u16> result;
    find_cuts(segments, result, tile_size);
    EXPECT_EQ(result, expected);
}

TEST(FindCutsTest, star_vertex_on_the_right_all_cuts)
{
    const auto tile_size = g_max_tile_size;

    const auto segments = make_line({
        { tile_size, 50 },
        { 10, 60 },
        { 10, 50 },
        { tile_size, 50 },
        { 10, 40 },
        { 10, 30 },
        { tile_size, 50 },
        { 10, 20 },
        { 10, 10 },
        { tile_size, 50 },
    });
    const std::vector<Segment2u16> expected;

    std::vector<Segment2u16> result;
    find_cuts(segments, result, tile_size);
    EXPECT_EQ(result, expected);
}

TEST(FindCutsTest, inverted_star_vertex_on_the_right_all_cuts)
{
    const auto tile_size = g_max_tile_size;

    const auto segments = make_line({
        { tile_size, 50 },
        { 10, 10 },
        { 10, 20 },
        { tile_size, 50 },
        { 10, 30 },
        { 10, 40 },
        { tile_size, 50 },
        { 10, 50 },
        { 10, 60 },
        { tile_size, 50 },
    });
    const auto expected = make_line({
        { tile_size, 50 },
        { tile_size, tile_size },
        { 0, tile_size },
        { 0, 0 },
        { tile_size, 0 },
        { tile_size, 50 },
    });

    std::vector<Segment2u16> result;
    find_cuts(segments, result, tile_size);
    EXPECT_EQ(result, expected);
}

TEST(FindCutsTest, collinear_simple)
{
    const auto tile_size = g_max_tile_size;

    {
        const auto segments = make_line({
            { 10, 0 },
            { 20, 0 },
            { 30, 0 },
            { 30, 10 },
            { 10, 0 },
        });
        const std::vector<Segment2u16> expected;

        std::vector<Segment2u16> result;
        find_cuts(segments, result, tile_size);
        EXPECT_EQ(result, expected);
    }

    {
        const auto segments = make_line({
            { 10, 0 },
            { 20, 0 },
            { 30, 0 },
        });
        const auto expected = make_line({
            { 30, 0 },
            { tile_size, 0 },
            { tile_size, tile_size },
            { 0, tile_size },
            { 0, 0 },
            { 10, 0 },
        });

        std::vector<Segment2u16> result;
        find_cuts(segments, result, tile_size);
        EXPECT_EQ(result, expected);
    }
}

TEST(FindCutsTest, collinear_zero_bottom)
{
    const auto tile_size = g_max_tile_size;

    {
        const auto segments = make_line({
            { 0, 0 },
            { 20, 0 },
            { 30, 0 },
            { 30, 10 },
            { 0, 0 },
        });
        const std::vector<Segment2u16> expected;

        std::vector<Segment2u16> result;
        find_cuts(segments, result, tile_size);
        EXPECT_EQ(result, expected);
    }

    {
        const auto segments = make_line({
            { 0, 0 },
            { 20, 0 },
            { 30, 0 },
        });
        const auto expected = make_line({
            { 30, 0 },
            { tile_size, 0 },
            { tile_size, tile_size },
            { 0, tile_size },
            { 0, 0 },
        });

        std::vector<Segment2u16> result;
        find_cuts(segments, result, tile_size);
        EXPECT_EQ(result, expected);
    }
}

TEST(FindCutsTest, collinear_zero_left)
{
    const auto tile_size = g_max_tile_size;

    {
        const auto segments = make_line({
            { 0, 30 },
            { 0, 20 },
            { 0, 0 },
            { 10, 30 },
            { 0, 30 },
        });
        const std::vector<Segment2u16> expected;

        std::vector<Segment2u16> result;
        find_cuts(segments, result, tile_size);
        EXPECT_EQ(result, expected);
    }

    {
        const auto segments = make_line({
            { 0, 30 },
            { 0, 20 },
            { 0, 0 },
        });
        const auto expected = make_line({
            { 0, 0 },
            { tile_size, 0 },
            { tile_size, tile_size },
            { 0, tile_size },
            { 0, 30 },
        });

        std::vector<Segment2u16> result;
        find_cuts(segments, result, tile_size);
        EXPECT_EQ(result, expected);
    }
}

} // namespace ka
