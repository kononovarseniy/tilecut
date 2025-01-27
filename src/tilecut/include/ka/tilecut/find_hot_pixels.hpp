#pragma once

#include <iterator>

#include <ka/exact/grid.hpp>
#include <ka/geometry_types/Vec2.hpp>

namespace ka
{

template <GridRounding rounding>
[[nodiscard]] Vec2s64 find_point_hot_pixel(const GridParameters & grid, const Vec2f64 point)
{
    return {
        .x = column_containing_position<rounding>(grid, point.x),
        .y = row_containing_position<rounding>(grid, point.y),
    };
}

template <GridRounding rounding>
auto find_tile_bounds_hot_pixels(
    const GridParameters & grid,
    const u16 tile_step,
    const Vec2f64 segment_start,
    const Vec2s64 segment_start_pixel,
    const Vec2f64 segment_stop,
    const Vec2s64 segment_stop_pixel,
    std::output_iterator<Vec2s64> auto pixels_it)
{
    const auto min_tile = [tile_step](const s64 a, const s64 b)
    {
        const auto value = std::min(a, b);
        if (value >= 0)
        {
            return ((value + tile_step - 1) / tile_step) * tile_step;
        }
        return -(-value / tile_step) * tile_step;
    };
    const auto max_tile = [tile_step](const s64 a, const s64 b)
    {
        const auto value = std::max(a, b);
        if (value >= 0)
        {
            return (value / tile_step) * tile_step;
        }
        return -((-value + tile_step - 1) / tile_step) * tile_step;
    };

    const auto min_x = min_tile(segment_start_pixel.x, segment_stop_pixel.x);
    const auto max_x = max_tile(segment_start_pixel.x, segment_stop_pixel.x);
    const auto min_y = min_tile(segment_start_pixel.y, segment_stop_pixel.y);
    const auto max_y = max_tile(segment_start_pixel.y, segment_stop_pixel.y);

    if (segment_start.x != segment_stop.x)
    {
        for (auto x = min_x; x <= max_x; x += tile_step)
        {
            if (border_between_coordinates(grid.cell_size, segment_start_pixel.x, segment_stop_pixel.x, x))
            {
                // clang-format off
                const auto y = column_border_intersection<rounding>(
                    grid,
                    segment_start.x,
                    segment_start.y,
                    segment_stop.x,
                    segment_stop.y,
                    x);
                // clang-format on
                *pixels_it++ = { x, y };
            }
        }
    }
    if (segment_start.y != segment_stop.y)
    {
        for (auto y = min_y; y <= max_y; y += tile_step)
        {
            if (border_between_coordinates(grid.cell_size, segment_start_pixel.y, segment_stop_pixel.y, y))
            {
                // clang-format off
                const auto x = row_border_intersection<rounding>(
                    grid,
                    segment_start.x,
                    segment_start.y,
                    segment_stop.x,
                    segment_stop.y,
                    y);
                // clang-format on
                *pixels_it++ = { x, y };
            }
        }
    }
    return pixels_it;
}

} // namespace ka
