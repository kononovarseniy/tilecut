#pragma once

#include <span>
#include <vector>

#include <geometry_types/Segment2.hpp>

namespace ka
{

/// @brief Restores cut segments, i.e. parts of the tile border that belong to the interior of the multipolygon.
/// @param segments segments of the multipolygon inside the tile.
/// @param result storage for result segments.
/// @param tile_size size of the tile.
void find_cuts(std::span<const Segment2u16> segments, std::vector<Segment2u16> & result, u16 tile_size) noexcept;

/// @brief Checks that the interior of the tile below the current tile contains some points of the same multipolygon.
/// One can use this info to find tiles completely covered by the multipolygon.
/// @param cut_segments tile border parts that belongs to the interior of a multipolygon.
/// @return True if there is a cut segment on the bottom border of the tile.
[[nodiscard]] bool open_on_the_bottom(std::span<const Segment2u16> cut_segments) noexcept;

} // namespace ka
