#pragma once

#include <span>
#include <vector>

#include <ka/geometry_types/Segment2.hpp>
#include <ka/geometry_types/Vec2.hpp>
#include <ka/tilecut/TileGrid.hpp>

namespace ka
{

struct Tile final
{
    Vec2s64 tile;
    std::span<const Segment2u16> segments;
};

/// @brief Groups segments by tile coordinates and abtains the sorted list of tile coordinates.
/// @note Vector unique_segments is modified by grouping segments by tile coordinates.
/// @param tile_grid tile grid.
/// @param unique_segments unique segments of multipolygon.
/// @param tile_segments container for segments in tile coordinates. Subranges of `tile_segments` are referenced by
/// items of `tiles` container.
/// @param tiles container for found tiles. Tiles are sorted by coordinates.
/// @note Segments that lie entirely on the boundary of a tile are considered to belong to the tile that is in the left
/// half-plane relative to that segment.
/// Thanks to this property, a tile will never contain a 2D part of a polygon.
/// In addition, this makes it a little easier to find the boundaries of a tile lying inside a polygon.
void collect_tiles(
    const TileGrid & tile_grid,
    std::vector<Segment2s64> & unique_segments,
    std::vector<Segment2u16> & tile_segments,
    std::vector<Tile> & tiles) noexcept;

} // namespace ka
