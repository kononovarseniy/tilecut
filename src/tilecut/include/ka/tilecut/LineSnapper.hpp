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

class LineSnapper final
{
public:
    template <
        GridRounding rounding,
        LineSnapperCoordinateHandler Handler,
        std::ranges::input_range In,
        std::output_iterator<typename Handler::OutputVertex> Out>
        requires std::same_as<typename Handler::InputVertex, std::ranges::range_value_t<In>>
    void snap_line(const TileCellGrid<rounding> & grid, In && line, Out out, const Handler & handler)
    {
        bool first = true;
        Vec2f64 prev_vertex;
        Vec2s64 prev_pixel;
        typename Handler::InputVertex prev_input;
        typename Handler::OutputVertex prev_output;

        for (const auto & curr_input : line)
        {
            const auto curr_vertex = handler.project(curr_input);
            const auto curr_pixel = grid.cell_of(curr_vertex);
            const auto curr_output = handler.transform(curr_input, curr_pixel);

            if (first)
            {
                first = false;
            }
            else
            {
                interior_pixels_.clear();

                grid.tile_boundary_intersection_cells(
                    { prev_vertex, curr_vertex },
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
            }

            *out++ = curr_output;

            prev_vertex = curr_vertex;
            prev_pixel = curr_pixel;
            prev_input = curr_input;
            prev_output = curr_output;
        }
    }

private:
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
