#include <algorithm>
#include <iterator>

#include <ka/common/assert.hpp>
#include <ka/exact/grid.hpp>
#include <ka/tilecut/HotPixelCollector.hpp>
#include <ka/tilecut/find_hot_pixels.hpp>

namespace ka
{

void HotPixelCollector::init(const GridParameters & grid, const u16 tile_step) noexcept
{
    AR_PRE(grid.desired_cell_size > 0.0);
    AR_PRE(grid.cell_size >= grid.desired_cell_size);
    AR_PRE(tile_step > 0);

    grid_ = grid;
    tile_step_ = tile_step;
    prev_vertex_.reset();
    hot_pixels_.clear();
}

void HotPixelCollector::new_contour() noexcept
{
    AR_PRE(grid_.has_value());
    AR_PRE(tile_step_ > 0);

    prev_vertex_.reset();
}

template <GridRounding rounding>
void HotPixelCollector::add_vertex_and_tile_cuts(const Vec2f64 & vertex) noexcept
{
    AR_PRE(grid_.has_value());
    AR_PRE(tile_step_ > 0);

    const auto pixel = hot_pixels_.emplace_back(find_point_hot_pixel<rounding>(*grid_, vertex));
    if (prev_vertex_.has_value())
    {
        AR_ASSERT(prev_pixel_.has_value());

        // clang-format off
        find_tile_bounds_hot_pixels<rounding>(
            *grid_,
            tile_step_,
            *prev_vertex_,
            *prev_pixel_,
            vertex,
            pixel,
            std::back_inserter(hot_pixels_));
        // clang-format on
    }
    prev_vertex_ = vertex;
    prev_pixel_ = pixel;
}

template void HotPixelCollector::add_vertex_and_tile_cuts<GridRounding::Cell>(const Vec2f64 & vertex) noexcept;
template void HotPixelCollector::add_vertex_and_tile_cuts<GridRounding::NearestNode>(const Vec2f64 & vertex) noexcept;

const HotPixelIndex & HotPixelCollector::build_index() noexcept
{
    AR_PRE(grid_.has_value());
    AR_PRE(tile_step_ > 0);
    AR_PRE(!hot_pixels_.empty());

    std::ranges::sort(hot_pixels_);
    const auto to_remove = std::ranges::unique(hot_pixels_);
    hot_pixels_.erase(to_remove.begin(), to_remove.end());

    index_.grid_ = &*grid_;
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

} // namespace ka
