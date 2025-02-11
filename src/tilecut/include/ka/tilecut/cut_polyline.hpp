#pragma once

#include <concepts>
#include <functional>
#include <ranges>
#include <type_traits>

#include <ka/geometry_types/Vec2.hpp>
#include <ka/tilecut/TileGrid.hpp>

namespace ka
{

template <typename T, typename Vertex>
concept VertexProject =
    std::invocable<T, const Vertex &> && std::convertible_to<std::invoke_result_t<T, const Vertex &>, Vec2s64>;

/// @brief Splits a polyline into parts, each of which lies entirely within a single tile. The minimum possible number
/// of cuts is not guaranteed, but in most situations this will be the case.
/// @param tile_grid defines the size of the tile.
/// @param line polyline represented as a range.
/// @param proj invocable converting polyline vertices to their Vec2s64 xy coordinates.
/// @param visitor invocable that will be called for each polyline part with Vec2s64 tile_coordinates and two iterators
/// to the first and last vertices of the part,
template <
    std::ranges::bidirectional_range Line,
    VertexProject<std::ranges::range_value_t<Line>> Proj = std::identity,
    std::invocable<const Vec2s64 &, std::ranges::iterator_t<Line>, std::ranges::iterator_t<Line>> Visitor>
void cut_polyline(const TileGrid & tile_grid, Line && line, Proj proj, Visitor visitor)
{
    const auto first = std::begin(line);
    const auto last = std::end(line);
    if (first == last)
    {
        return;
    }

    auto start = first;
    while (std::next(start) != last)
    {
        const auto segment_start_xy = std::invoke(proj, *start);
        const auto segment_stop = std::ranges::find_if(
            std::next(start),
            last,
            [&](const auto & xy)
            {
                return xy != segment_start_xy;
            },
            proj);
        if (segment_stop == last)
        {
            // If the part is not the first, then all points with coinciding projections were assigned to the previous
            // part.
            AR_ASSERT(start == first);
            std::invoke(visitor, tile_grid.tile_of(segment_start_xy), first, last);
            return;
        }
        const auto segment_stop_xy = std::invoke(proj, *segment_stop);
        // Determining tile coordinates by the first segment may lead to unnecessary cuts, but this is the easiest way
        // to unambiguously determine which tile we are working with.
        const auto current_tile = tile_grid.tile_of({ segment_start_xy, segment_stop_xy });
        AR_ASSERT(!tile_grid.strictly_outside(current_tile, segment_start_xy));
        AR_ASSERT(!tile_grid.strictly_outside(current_tile, segment_stop_xy));

        const auto stop = std::ranges::find_if(
            std::next(segment_stop),
            last,
            [&](const auto & xy)
            {
                return tile_grid.strictly_outside(current_tile, xy);
            },
            proj);

        std::invoke(visitor, current_tile, start, stop);

        start = std::prev(stop);
    }
}

} // namespace ka
