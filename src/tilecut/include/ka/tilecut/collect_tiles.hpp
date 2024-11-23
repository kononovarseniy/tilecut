#pragma once

#include <span>
#include <vector>

#include <ka/geometry_types/Segment2.hpp>
#include <ka/geometry_types/Vec2.hpp>

namespace ka
{

struct Tile final
{
    Vec2s32 tile;
    std::span<const Segment2u16> segments;
};

void collect_tiles(
    std::vector<Segment2s64> & unique_segments,
    std::vector<Segment2u16> & tile_segments,
    std::vector<Tile> & tiles,
    u16 tile_size) noexcept;

} // namespace ka
