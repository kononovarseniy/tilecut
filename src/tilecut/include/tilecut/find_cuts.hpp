#pragma once

#include <span>
#include <vector>

#include <geometry_types/Segment2.hpp>

namespace r7
{

/// @brief Restores cut segments, i.e. parts of the tile border that belong to the interior of the multipolygon.
/// @param segments segments of the multipolygon inside the tile.
/// @param result storage for result segments.
/// @param tile_size size of the tile.
void find_cuts(std::span<const Segment2u16> segments, std::vector<Segment2u16> & result, u16 tile_size) noexcept;

} // namespace r7
