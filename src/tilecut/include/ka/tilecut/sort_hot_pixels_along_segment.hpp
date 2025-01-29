#pragma once

#include <algorithm>
#include <ranges>

#include <ka/geometry_types/Vec2.hpp>
#include <ka/tilecut/HotPixelOrder.hpp>
#include <ka/tilecut/hot_pixel_less.hpp>

namespace ka
{

/// @brief Sorts hot pixels by restoring the order in which they intersect the snap-rounded segment.
/// It is assumed that all pixels intersect the original segment.
/// @note If the above assumption is satisfied, then there is no ambiguity in the hot pixel order.
template <std::ranges::random_access_range R, typename Proj = std::identity>
// clang-format off
    requires std::sortable<
        std::ranges::iterator_t<R>,
        hot_pixel_less<HotPixelOrder::Ascending, HotPixelOrder::Ascending>,
        Proj>
constexpr void sort_hot_pixels_along_segment(
    R && pixels,
    const Vec2s64 segment_start_pixel,
    const Vec2s64 segment_stop_pixel,
    Proj proj = {})
// clang-format on
{
    const bool horizontal_ascending = segment_start_pixel.x <= segment_stop_pixel.x;
    const bool vertical_ascending = segment_start_pixel.y <= segment_stop_pixel.y;
    if (horizontal_ascending)
    {
        if (vertical_ascending)
        {
            std::ranges::sort(
                std::forward<R>(pixels),
                hot_pixel_less<HotPixelOrder::Ascending, HotPixelOrder::Ascending> {},
                proj);
        }
        else
        {
            std::ranges::sort(
                std::forward<R>(pixels),
                hot_pixel_less<HotPixelOrder::Ascending, HotPixelOrder::Descending> {},
                proj);
        }
    }
    else
    {
        if (vertical_ascending)
        {
            std::ranges::sort(
                std::forward<R>(pixels),
                hot_pixel_less<HotPixelOrder::Descending, HotPixelOrder::Ascending> {},
                proj);
        }
        else
        {
            std::ranges::sort(
                std::forward<R>(pixels),
                hot_pixel_less<HotPixelOrder::Descending, HotPixelOrder::Descending> {},
                proj);
        }
    }
}

} // namespace ka
