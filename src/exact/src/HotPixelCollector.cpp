#include <algorithm>

#include <ka/common/assert.hpp>
#include <ka/exact/HotPixelCollector.hpp>
#include <ka/exact/grid.hpp>

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

    const auto pixel = hot_pixels_.emplace_back(
        column_containing_position<rounding>(*grid_, vertex.x),
        column_containing_position<rounding>(*grid_, vertex.y));
    if (prev_vertex_.has_value())
    {
        AR_ASSERT(prev_pixel_.has_value());

        const auto min_tile = [this](const s64 a, const s64 b)
        {
            const auto value = std::min(a, b);
            if (value >= 0)
            {
                return ((value + tile_step_ - 1) / tile_step_) * tile_step_;
            }
            return -(-value / tile_step_) * tile_step_;
        };
        const auto max_tile = [this](const s64 a, const s64 b)
        {
            const auto value = std::max(a, b);
            if (value >= 0)
            {
                return (value / tile_step_) * tile_step_;
            }
            return -((-value + tile_step_ - 1) / tile_step_) * tile_step_;
        };

        const auto min_x = min_tile(prev_pixel_->x, pixel.x);
        const auto max_x = max_tile(prev_pixel_->x, pixel.x);
        const auto min_y = min_tile(prev_pixel_->y, pixel.y);
        const auto max_y = max_tile(prev_pixel_->y, pixel.y);

        if (prev_vertex_->x != vertex.x)
        {
            for (auto x = min_x; x <= max_x; x += tile_step_)
            {
                if (border_between_coordinates(*grid_, prev_vertex_->x, vertex.x, x))
                {
                    // clang-format off
                    const auto y = column_border_intersection<rounding>(
                        *grid_,
                        prev_vertex_->x,
                        prev_vertex_->y,
                        vertex.x,
                        vertex.y,
                        x);
                    // clang-format on
                    hot_pixels_.emplace_back(x, y);
                }
            }
        }
        if (prev_vertex_->y != vertex.y)
        {
            for (auto y = min_y; y <= max_y; y += tile_step_)
            {
                if (border_between_coordinates(*grid_, prev_vertex_->y, vertex.y, y))
                {
                    // clang-format off
                    const auto x = row_border_intersection<rounding>(
                        *grid_,
                        prev_vertex_->x,
                        prev_vertex_->y,
                        vertex.x,
                        vertex.y,
                        y);
                    // clang-format on
                    hot_pixels_.emplace_back(x, y);
                }
            }
        }
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
            index_.columns_.emplace_back(current_x, std::span(hot_pixels_).subspan(span_start, i - span_start));
            current_x = hot_pixels_[i].x;
            span_start = i;
        }
    }
    index_.columns_.emplace_back(current_x, std::span(hot_pixels_).subspan(span_start));
    return index_;
}

} // namespace ka
