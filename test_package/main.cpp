#include <cstdlib>
#include <vector>

#include <ka/common/assert.hpp>
#include <ka/geometry_types/Segment2.hpp>
#include <ka/tilecut/collect_tiles.hpp>
#include <ka/tilecut/filter_segments.hpp>
#include <ka/tilecut/find_cuts.hpp>

int main(int argc, char * argv[])
{
    const auto tile_size = 10000;
    // clang-format off
    std::vector<ka::Segment2s64> segments = {
        { { 50, 50 }, { 100, 100 } },
        { { 100, 100 }, { 50, 50 } },
        { { 1000, 1000 }, { 2000, 1000 } },
        { { 2000, 1000 }, { 1500, 2000 } },
        { { 1500, 2000 }, { 1000, 1000 } },
    };
    // clang-format on

    filter_segments(segments);

    std::vector<ka::Segment2u16> tile_segments_storage;
    std::vector<ka::Tile> tiles;
    collect_tiles(segments, tile_segments_storage, tiles, tile_size);

    std::vector<ka::Segment2u16> cut_segments;
    for (const auto & tile : tiles)
    {
        cut_segments.clear();
        find_cuts(tile.segments, cut_segments, tile_size);
        if (!cut_segments.empty())
        {
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}
