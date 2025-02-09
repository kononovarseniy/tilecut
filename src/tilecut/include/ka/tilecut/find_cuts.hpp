#pragma once

#include <span>
#include <vector>

#include <ka/geometry_types/Segment2.hpp>
#include <ka/tilecut/TileGrid.hpp>

namespace ka
{

/// @brief Restores cut segments, i.e. parts of the tile border that belong to the interior of the multipolygon.
/// @param tile_grid defines the size of the tile.
/// @param segments segments of the multipolygon inside the tile.
/// @param result storage for result segments.
void find_cuts(
    const TileGrid & tile_grid,
    std::span<const Segment2u16> segments,
    std::vector<Segment2u16> & result) noexcept;

/// @brief Checks that the interior of the tile below the current tile contains some points of the same multipolygon.
/// One can use this info to find tiles completely covered by the multipolygon.
/// @param cut_segments tile border parts that belongs to the interior of a multipolygon.
/// @return True if there is a cut segment on the bottom border of the tile.
[[nodiscard]] bool open_on_the_bottom(std::span<const Segment2u16> cut_segments) noexcept;

} // namespace ka
