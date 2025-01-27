#pragma once

#include <optional>
#include <vector>

#include <ka/exact/GridParameters.hpp>
#include <ka/exact/GridRounding.hpp>
#include <ka/geometry_types/Vec2.hpp>
#include <ka/tilecut/HotPixelIndex.hpp>

namespace ka
{

//! Collects hot pixels to index.
class HotPixelCollector final
{
public:
    //! Reset state and set grid parameters.
    void init(const GridParameters & grid, u16 tile_step) noexcept;

    //! Start new contour.
    void new_contour() noexcept;

    //! Add hot pixels around the endpoints of the new segment
    //! and around the intersection points of the segment and tiles.
    template <GridRounding rounding>
    void add_vertex_and_tile_cuts(const Vec2f64 & vertex) noexcept;

    //! The index is invalidated on HotPixelCollector modifications.
    [[nodiscard]] const HotPixelIndex & build_index() noexcept;

private:
    std::optional<GridParameters> grid_;
    u16 tile_step_ = 0;
    std::optional<Vec2f64> prev_vertex_;
    std::optional<Vec2s64> prev_pixel_;
    std::vector<Vec2s64> hot_pixels_;
    HotPixelIndex index_;
};

} // namespace ka
