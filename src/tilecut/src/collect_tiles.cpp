#include <algorithm>

#include <ka/common/assert.hpp>
#include <ka/common/cast.hpp>
#include <ka/common/fixed.hpp>
#include <ka/tilecut/collect_tiles.hpp>

namespace ka
{

inline namespace
{

[[nodiscard]] s32 coordinate_tile(const s64 value, const u16 tile_size)
{
    if (value >= 0)
    {
        return exact_cast<s32>(value / tile_size);
    }
    return exact_cast<s32>(-((-value + tile_size - 1) / tile_size));
};

[[nodiscard]] Vec2s32 segment_tile(const Segment2s64 & segment, const u16 tile_size)
{
    AR_PRE(segment.a != segment.b);
    const auto tile_x = std::min(coordinate_tile(segment.a.x, tile_size), coordinate_tile(segment.b.x, tile_size));
    const auto tile_y = std::min(coordinate_tile(segment.a.y, tile_size), coordinate_tile(segment.b.y, tile_size));
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

void collect_tiles(
    std::vector<Segment2s64> & unique_segments,
    std::vector<Segment2u16> & tile_segments,
    std::vector<Tile> & tiles,
    const u16 tile_size) noexcept
{
    tile_segments.clear();
    tiles.clear();
    if (unique_segments.empty())
    {
        return;
    }

    const auto segment_to_tile = [&](const auto & segment)
    {
        const auto tile = segment_tile(segment, tile_size);
        AR_ASSERT(segment.a.x >= tile_size * tile.x);
        AR_ASSERT(segment.a.x <= tile_size * tile.x + tile_size);
        AR_ASSERT(segment.a.y >= tile_size * tile.y);
        AR_ASSERT(segment.a.y <= tile_size * tile.y + tile_size);
        AR_ASSERT(segment.b.x >= tile_size * tile.x);
        AR_ASSERT(segment.b.x <= tile_size * tile.x + tile_size);
        AR_ASSERT(segment.b.y >= tile_size * tile.y);
        AR_ASSERT(segment.b.y <= tile_size * tile.y + tile_size);
        return tile;
    };

    std::ranges::sort(unique_segments, {}, segment_to_tile);

    tile_segments.reserve(unique_segments.size());

    auto flush_tile = [local_it = tile_segments.begin(), &tiles, &tile_segments](const Vec2s32 tile) mutable
    {
        tiles.push_back({ .tile = tile, .segments { local_it, tile_segments.end() } });
        local_it = tile_segments.end();
    };

    auto prev_tile = segment_to_tile(unique_segments.front());

    for (const auto & segment : unique_segments)
    {
        const auto tile = segment_to_tile(segment);
        if (tile != prev_tile)
        {
            flush_tile(prev_tile);
            prev_tile = tile;
        }
        tile_segments.push_back({
            { exact_cast<u16>(segment.a.x - tile.x * tile_size), exact_cast<u16>(segment.a.y - tile.y * tile_size) },
            { exact_cast<u16>(segment.b.x - tile.x * tile_size), exact_cast<u16>(segment.b.y - tile.y * tile_size) },
        });
    }
    flush_tile(prev_tile);
}

} // namespace ka
