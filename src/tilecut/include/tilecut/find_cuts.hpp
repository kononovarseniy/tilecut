#pragma once

#include <span>
#include <vector>

#include <geometry_types/Segment2.hpp>

namespace r7
{

void find_cuts(std::span<const Segment2u16> segments, std::vector<Segment2u16> & result, u16 tile_size) noexcept;

} // namespace r7
