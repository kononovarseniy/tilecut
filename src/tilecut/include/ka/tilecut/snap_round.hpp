#pragma once

#include <ranges>

#include <ka/exact/GridRounding.hpp>
#include <ka/exact/grid.hpp>
#include <ka/geometry_types/Vec2.hpp>
#include <ka/tilecut/HotPixelIndex.hpp>
#include <ka/tilecut/TileCellGrid.hpp>

namespace ka
{

//! Performs countour snap rounding using specified hot pixels.
template <GridRounding rounding, std::ranges::input_range In, std::output_iterator<Vec2s64> Out>
Out snap_round(const TileCellGrid<rounding> & grid, const HotPixelIndex & hot_pixels, In && line, Out output)
{
    Vec2f64 prev_vertex;
    Vec2s64 prev_pixel;

    bool first = true;
    for (const auto & vertex : line)
    {
        const Vec2s64 pixel = grid.cell_of(vertex);
        if (first)
        {
            *output++ = pixel;
            prev_vertex = vertex;
            prev_pixel = pixel;
            first = false;
            continue;
        }

        const auto predicate = [&](const auto & hot_pixel)
        {
            AR_PRE(std::min(prev_pixel.x, pixel.x) <= hot_pixel.x);
            AR_PRE(hot_pixel.x <= std::max(prev_pixel.x, pixel.x));
            AR_PRE(std::min(prev_pixel.y, pixel.y) <= hot_pixel.y);
            AR_PRE(hot_pixel.y <= std::max(prev_pixel.y, pixel.y));

            if (hot_pixel == prev_pixel || hot_pixel == pixel)
            {
                // Endpoints are added explicitly to reduce the amount of pixel repetitions.
                return false;
            }
            return grid.line_intersects_cell({ prev_vertex, vertex }, hot_pixel);
        };
        const bool horizontal_ascending = prev_pixel.x <= pixel.x;
        const bool vertical_ascending = prev_pixel.y <= pixel.y;
        // clang-format off
        using enum HotPixelOrder;
        output = horizontal_ascending
            ? vertical_ascending
                ? hot_pixels.find_if<Ascending, Ascending>(prev_pixel.x, pixel.x, prev_pixel.y, pixel.y, output, predicate)
                : hot_pixels.find_if<Ascending, Descending>(prev_pixel.x, pixel.x, pixel.y, prev_pixel.y, output, predicate)
            : vertical_ascending
                ? hot_pixels.find_if<Descending, Ascending>(pixel.x, prev_pixel.x, prev_pixel.y, pixel.y, output, predicate)
                : hot_pixels.find_if<Descending, Descending>(pixel.x, prev_pixel.x, pixel.y, prev_pixel.y, output, predicate);
        // clang-format on
        *output++ = pixel;
        prev_vertex = vertex;
        prev_pixel = pixel;
    }
    return output;
}

} // namespace ka
