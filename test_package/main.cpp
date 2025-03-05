#include <cstdlib>
#include <vector>

#include <ka/common/assert.hpp>
#include <ka/exact/GridRounding.hpp>
#include <ka/geometry_types/Segment2.hpp>
#include <ka/tilecut/HotPixelCollector.hpp>
#include <ka/tilecut/TileCellGrid.hpp>
#include <ka/tilecut/collect_tiles.hpp>
#include <ka/tilecut/filter_segments.hpp>
#include <ka/tilecut/find_cuts.hpp>
#include <ka/tilecut/snap_round.hpp>
#include <ka_test/grid.hpp>

int main(int argc, char * argv[])
{
    auto grid_params = ka::g_embedded_grid;
    grid_params.cell_size = 1.0;
    const ka::u16 tile_size = 10000;
    ka::TileCellGrid<ka::GridRounding::NearestNode> grid(grid_params, tile_size);

    // Input geometry.
    std::vector<std::vector<ka::Vec2f64>> contours = {
        { { 50, 50.01 }, { 100, 100 }, { 50, 50.01 } },
        { { 1000, 1000 }, { 2000, 1000 }, { 1500, 2000 }, { 1000, 1000 } },
    };

    // Collect all relevant hot pixels.
    ka::HotPixelCollector hot_pixel_collector;
    hot_pixel_collector.reset();

    for (const auto & contour : contours)
    {
        hot_pixel_collector.add_tile_snapped_polyline(grid, contour);
    }

    // Snap rounding.
    std::vector<ka::Segment2s64> segments;
    const auto & hot_pixels = hot_pixel_collector.build_index();
    std::vector<ka::Vec2s64> coarse_points;
    for (const auto & contour : contours)
    {
        coarse_points.clear();
        snap_round(grid, hot_pixels, contour, std::back_inserter(coarse_points));
        auto prev = coarse_points.front();
        for (auto it = std::next(coarse_points.begin()); it != coarse_points.end(); prev = *it++)
        {
            segments.push_back({ prev, *it });
        }
    }

    // Remove duplicates inplace.
    filter_segments(segments);

    // Group segments by tiles.
    std::vector<ka::Segment2u16> tile_segments_storage;
    std::vector<ka::Tile> tiles;
    collect_tiles(grid.tiles(), segments, tile_segments_storage, tiles);

    // Find cut segents.
    std::vector<ka::Segment2u16> cut_segments;
    for (const auto & tile : tiles)
    {
        cut_segments.clear();
        find_cuts(grid.tiles(), tile.segments, cut_segments);

        if (!cut_segments.empty())
        {
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}
