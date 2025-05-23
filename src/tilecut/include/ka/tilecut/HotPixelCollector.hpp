#pragma once

#include <concepts>
#include <ranges>
#include <vector>

#include <ka/exact/GridRounding.hpp>
#include <ka/geometry_types/Vec2.hpp>
#include <ka/tilecut/HotPixelIndex.hpp>
#include <ka/tilecut/TileCellGrid.hpp>

namespace ka
{

/// @brief Collects hot pixels to index.
class HotPixelCollector final
{
public:
    /// @brief Resets the collector to its original state. Clears all accumulated hot pixels.
    void reset() noexcept
    {
        hot_pixels_.clear();
    }

    /// @brief Adds hot pixels corresponding to vertices and intersections with tile boundaries of the polyline.
    /// @param grid defines the tile grid and cell grid sizes.
    /// @param polyline vertices of the polyline.
    template <GridRounding rounding, std::ranges::input_range In>
        requires std::same_as<std::ranges::range_value_t<In>, Vec2f64>
    void add_tile_snapped_polyline(const TileCellGrid<rounding> & grid, In && polyline) noexcept
    {
        Vec2f64 prev_vertex {};
        Vec2s64 prev_pixel {};
        bool first = true;

        for (const auto & vertex : polyline)
        {
            const auto pixel = hot_pixels_.emplace_back(grid.cell_of(vertex));
            if (first)
            {
                first = false;
            }
            else
            {
                grid.tile_boundary_intersection_cells(
                    { prev_vertex, vertex },
                    { prev_pixel, pixel },
                    std::back_inserter(hot_pixels_));
            }
            prev_vertex = vertex;
            prev_pixel = pixel;
        }
    }

    /// @brief The index is invalidated on HotPixelCollector modifications.
    [[nodiscard]] const HotPixelIndex & build_index() noexcept
    {
        AR_PRE(!hot_pixels_.empty());

        std::ranges::sort(hot_pixels_);
        const auto to_remove = std::ranges::unique(hot_pixels_);
        hot_pixels_.erase(to_remove.begin(), to_remove.end());

        index_.columns_.clear();

        size_t span_start = 0;
        auto current_x = hot_pixels_.front().x;
        for (size_t i = 0; i < hot_pixels_.size(); ++i)
        {
            if (hot_pixels_[i].x != current_x)
            {
                index_.columns_.push_back({ current_x, std::span(hot_pixels_).subspan(span_start, i - span_start) });
                current_x = hot_pixels_[i].x;
                span_start = i;
            }
        }
        index_.columns_.push_back({ current_x, std::span(hot_pixels_).subspan(span_start) });
        return index_;
    }

private:
    std::vector<Vec2s64> hot_pixels_;
    HotPixelIndex index_;
};

} // namespace ka
