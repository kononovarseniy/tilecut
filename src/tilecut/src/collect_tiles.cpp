#include <algorithm>

#include <common/assert.hpp>
#include <common/fixed.hpp>
#include <tilecut/collect_tiles.hpp>

namespace r7
{

inline namespace
{

[[nodiscard]] s32 coordinate_tile(const s64 value, const u16 tile_size)
{
    if (value >= 0)
    {
        return value / tile_size;
    }
    return -((-value + tile_size - 1) / tile_size);
};

[[nodiscard]] Vec2s32 segment_tile(const Segment2s64 & segment, const u16 tile_size)
{
    AR_PRE(segment.a != segment.b);
    const auto tile_x = coordinate_tile(segment.a.x, tile_size);
    const auto tile_y = coordinate_tile(segment.a.y, tile_size);
    if (segment.a.x == segment.b.x && segment.a.x == tile_x * tile_size)
    {
        return {
            segment.a.y < segment.b.y ? tile_x - 1 : tile_x,
            tile_y,
        };
    }
    if (segment.a.y == segment.b.y && segment.a.y == tile_y * tile_size)
    {
        return {
            tile_x,
            segment.a.x < segment.b.x ? tile_y : tile_y - 1,
        };
    }
    return { tile_x, tile_y };
}

} // namespace

void collect_tiles(std::vector<Segment2s64> & unique_segments, std::vector<Tile> & tiles, const u16 tile_size) noexcept
{
    const auto segment_to_tile = [&](const auto & segment)
    {
        return segment_tile(segment, tile_size);
    };

    std::ranges::sort(unique_segments, {}, segment_to_tile);

    tiles.clear();
    auto it = unique_segments.begin();
    while (it != unique_segments.end())
    {
        auto next = std::ranges::adjacent_find(unique_segments, std::not_equal_to<> {}, segment_to_tile);
        if (next != unique_segments.end())
        {
            ++next;
        }
        tiles.push_back({
            .tile = segment_to_tile(*it),
            .segments { it, next },
        });
        it = next;
    }
}

} // namespace r7
