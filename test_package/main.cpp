#include <cstdlib>
#include <vector>

#include <ka/common/assert.hpp>
#include <ka/exact/GridRounding.hpp>
#include <ka/exact/HotPixelCollector.hpp>
#include <ka/exact/snap_round.hpp>
#include <ka/geometry_types/Segment2.hpp>
#include <ka/tilecut/collect_tiles.hpp>
#include <ka/tilecut/filter_segments.hpp>
#include <ka/tilecut/find_cuts.hpp>

#include "embedded_grid.hpp"

int main(int argc, char * argv[])
{
    auto grid = ka::test::g_embedded_grid;
    grid.cell_size = 1.0;
    const auto tile_size = 10000;

    // Input geometry.
    std::vector<std::vector<ka::Vec2f64>> contours = {
        { { 50, 50.01 }, { 100, 100 }, { 50, 50.01 } },
        { { 1000, 1000 }, { 2000, 1000 }, { 1500, 2000 }, { 1000, 1000 } },
    };

    // Collect all relevant hot pixels.
    ka::HotPixelCollector hot_pixel_collector;
    hot_pixel_collector.init(grid, tile_size);

    for (const auto & contour : contours)
    {
        hot_pixel_collector.new_contour();
        for (const auto & point : contour)
        {
            hot_pixel_collector.add_vertex_and_tile_cuts<ka::GridRounding::NearestNode>(point);
        }
    }

    // Snap rounding.
    std::vector<ka::Segment2s64> segments;
    const auto & hot_pixels = hot_pixel_collector.build_index();
    std::vector<ka::Vec2s64> coarse_points;
    for (const auto & contour : contours)
    {
        coarse_points.clear();
        snap_round<ka::GridRounding::NearestNode>(hot_pixels, contour, std::back_inserter(coarse_points));
        auto prev = coarse_points.front();
        for (auto it = std::next(coarse_points.begin()); it != coarse_points.end(); prev = *it++)
        {
            segments.emplace_back(prev, *it);
        }
    }

    // Remove duplicates inplace.
    filter_segments(segments);

    // Group segments by tiles.
    std::vector<ka::Segment2u16> tile_segments_storage;
    std::vector<ka::Tile> tiles;
    collect_tiles(segments, tile_segments_storage, tiles, tile_size);

    // Find cut segents.
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
