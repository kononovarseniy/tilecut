#pragma once

#include <concepts>
#include <iterator>
#include <ranges>
#include <type_traits>
#include <vector>

#include <ka/common/fixed.hpp>
#include <ka/exact/GridRounding.hpp>
#include <ka/geometry_types/Vec2.hpp>
#include <ka/tilecut/LineSnapperCoordinateHandler.hpp>
#include <ka/tilecut/TileCellGrid.hpp>
#include <ka/tilecut/lerp_along_segment.hpp>
#include <ka/tilecut/sort_hot_pixels_along_segment.hpp>

namespace ka
{

/// @brief Wraps polyline snapping to avoid allocations.
class LineSnapper final
{
public:
    /// @brief Snaps a polyline to the grid at vertices and tile boundary intersections.
    /// @param grid defines cell and tile grids.
    /// @param handler handles coordinate transformation and interpolation.
    /// @param line input range of polyline points.
    /// @param out beginning of the destination points container.
    template <
        GridRounding rounding,
        LineSnapperCoordinateHandler H,
        std::ranges::input_range In,
        std::output_iterator<typename H::OutputVertex> Out>
        requires std::same_as<typename H::InputVertex, std::ranges::range_value_t<In>>
    void snap_line(const TileCellGrid<rounding> & grid, const H & handler, In && line, Out out)
    {
        static_assert(
            std::assignable_from<typename H::InputVertex &, std::ranges::range_reference_t<In>> &&
                std::constructible_from<typename H::InputVertex, std::ranges::range_reference_t<In>>,
            "InputVertex must be assignable and constructible from the range reference type. "
            "Consider making InputVertex copy assignable or using rvalue range");

        auto it = std::ranges::begin(line);
        const auto last = std::ranges::end(line);

        if (it == last)
        {
            return;
        }
        auto prev_input = *it++;
        auto prev_proj = handler.project(prev_input);
        auto prev_pixel = grid.cell_of(prev_proj);
        auto prev_output = handler.transform(prev_input, prev_pixel);

        *out++ = prev_output;

        for (; it != last; ++it)
        {
            auto && curr_input = *it;
            auto curr_proj = handler.project(curr_input);
            auto curr_pixel = grid.cell_of(curr_proj);
            auto curr_output = handler.transform(curr_input, curr_pixel);

            interior_pixels_.clear();

            grid.tile_boundary_intersection_cells(
                { prev_proj, curr_proj },
                { prev_pixel, curr_pixel },
                std::back_inserter(interior_pixels_));

            sort_hot_pixels_along_segment(interior_pixels_, prev_pixel, curr_pixel);

            const auto unique_end = std::ranges::unique(interior_pixels_);
            interior_pixels_.erase(unique_end.begin(), unique_end.end());

            const auto transform_result = std::ranges::transform(
                strictly_interior_pixels(prev_pixel, curr_pixel, interior_pixels_),
                out,
                [&](const auto & curr_pixel)
                {
                    return handler.interpolate(prev_input, prev_output, curr_input, curr_output, curr_pixel);
                });
            out = transform_result.out;

            *out++ = curr_output;

            prev_proj = std::move(curr_proj);
            prev_pixel = std::move(curr_pixel);
            prev_input = std::forward<decltype(curr_input)>(curr_input);
            prev_output = std::move(curr_output);
        }
    }

private:
    /// @brief Pixels that do not include segment endpoints.
    /// @param start pixel containing first segment endpoint.
    /// @param stop pixel containing second segment endpoint.
    /// @param pixels pixels on the segment ordered by distance from first segment endpoint.
    [[nodiscard]] static constexpr std::span<Vec2s64> strictly_interior_pixels(
        const Vec2s64 start,
        const Vec2s64 stop,
        std::span<Vec2s64> pixels)
    {
        if (!pixels.empty() && pixels.front() == start)
        {
            pixels = pixels.subspan(1);
        }
        if (!pixels.empty() && pixels.back() == stop)
        {
            pixels = pixels.subspan(0, pixels.size() - 1);
        }
        return pixels;
    }

private:
    std::vector<Vec2s64> interior_pixels_;
};

} // namespace ka
