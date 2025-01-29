#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <vector>

#include <ka/geometry_types/Vec2.hpp>
#include <ka/tilecut/sort_hot_pixels_along_segment.hpp>

namespace ka
{

using ::testing::ElementsAreArray;

TEST(SortHotPixelsAlongSegmentTest, ordering_test)
{
    std::vector<Vec2s64> pixels = {
        { -100, 100 },
        { 0, 0 },
    };
    std::vector<Vec2s64> expected = {
        { 0, 0 },
        { -100, 100 },
    };
    const Vec2s64 start { 0, 0 };
    const Vec2s64 stop { -1000, 1000 };
    sort_hot_pixels_along_segment(pixels, start, stop);
    EXPECT_THAT(pixels, ElementsAreArray(expected));
}

TEST(SortHotPixelsAlongSegmentTest, ordering_partial_tie_test)
{
    std::vector<Vec2s64> pixels = {
        { 0, 0 },
        { 0, 1 },
        { 1, 1 },
        { 2, 1 },
    };
    std::vector<Vec2s64> expected = {
        { 0, 0 },
        { 0, 1 },
        { 1, 1 },
        { 2, 1 },
    };
    const Vec2s64 start { 0, 0 };
    const Vec2s64 stop { 1000, 1000 };
    sort_hot_pixels_along_segment(pixels, start, stop);
    EXPECT_THAT(pixels, ElementsAreArray(expected));
}

TEST(SortHotPixelsAlongSegmentTest, projection_test)
{
    std::vector<Vec2u16> pixels = {
        { 100, 100 },
        { 0, 0 },
    };
    std::vector<Vec2u16> expected = {
        { 0, 0 },
        { 100, 100 },
    };
    const Vec2s64 start { 0, 0 };
    const Vec2s64 stop { 1000, 1000 };
    sort_hot_pixels_along_segment(
        pixels,
        start,
        stop,
        [](const auto item)
        {
            return Vec2s64 { item.x, item.y };
        });
    EXPECT_THAT(pixels, ElementsAreArray(expected));
}

} // namespace ka
