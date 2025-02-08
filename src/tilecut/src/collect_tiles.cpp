#include <algorithm>

#include <ka/geometry_types/Vec2.hpp>
#include <ka/tilecut/collect_tiles.hpp>

namespace ka
{

void collect_tiles(
    const TileGrid & tile_grid,
    std::vector<Segment2s64> & unique_segments,
    std::vector<Segment2u16> & tile_segments,
    std::vector<Tile> & tiles) noexcept
{
    tile_segments.clear();
    tiles.clear();
    if (unique_segments.empty())
    {
        return;
    }

    std::ranges::sort(
        unique_segments,
        {},
        [&](const auto & segment)
        {
            return tile_grid.tile_of(segment);
        });

    tile_segments.reserve(unique_segments.size());

    auto flush_tile = [local_it = tile_segments.begin(), &tiles, &tile_segments](const Vec2s64 tile) mutable
    {
        tiles.push_back({ .tile = tile, .segments { local_it, tile_segments.end() } });
        local_it = tile_segments.end();
    };

    auto prev_tile = tile_grid.tile_of(unique_segments.front());

    for (const auto & segment : unique_segments)
    {
        const auto tile = tile_grid.tile_of(segment);
        if (tile != prev_tile)
        {
            flush_tile(prev_tile);
            prev_tile = tile;
        }
        tile_segments.push_back(tile_grid.local_coordinates(tile, segment));
    }
    flush_tile(prev_tile);
}

} // namespace ka
