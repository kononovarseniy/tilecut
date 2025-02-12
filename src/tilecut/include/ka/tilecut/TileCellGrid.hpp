#pragma once

#include <algorithm>
#include <iterator>

#include <ka/exact/GridParameters.hpp>
#include <ka/exact/GridRounding.hpp>
#include <ka/exact/grid.hpp>
#include <ka/geometry_types/Segment2.hpp>
#include <ka/geometry_types/Vec2.hpp>
#include <ka/tilecut/TileGrid.hpp>

namespace ka
{

/// @brief A helper class that provides methods for mapping geometry to the grid cells it passes through.
template <GridRounding rounding_>
class TileCellGrid final
{
public:
    constexpr static auto rounding = rounding_;

public:
    TileCellGrid(const GridParameters & grid, const u16 tile_size) noexcept
        : grid_ { grid }
        , tile_grid_ { tile_size }
    {
        AR_PRE(grid.desired_cell_size > 0.0);
        AR_PRE(grid.cell_size >= grid.desired_cell_size);
    }

public:
    /// @brief Returns the coordinates of the grid cell containing the given point.
    [[nodiscard]] Vec2s64 cell_of(const Vec2f64 point) const noexcept
    {
        return {
            .x = column_containing_position<rounding>(grid_, point.x),
            .y = row_containing_position<rounding>(grid_, point.y),
        };
    }

    /// @brief Finds all grid cells where the given segment intersects tile boundaries.
    /// @param segment Original segment.
    /// @param segment_cells Original segment snapped to grid. Used for optimization, usually this value is already
    /// calculated by the caller.
    /// @param out_it Output iterator to the beginning of the destination.
    /// @return Output iterator to the end of destination.
    template <std::output_iterator<Vec2s64> Out>
    Out tile_boundary_intersection_cells(const Segment2f64 segment, const Segment2s64 segment_cells, Out out_it) const
    {
        AR_PRE(cell_of(segment.a) == segment_cells.a);
        AR_PRE(cell_of(segment.b) == segment_cells.b);
        const auto [min_x, max_x, min_y, max_y] = tiles().intersected_boundaries_ranges(segment_cells);

        if (segment.a.x != segment.b.x)
        {
            for (auto x = min_x; x <= max_x; x += tiles().tile_size())
            {
                if (border_between_coordinates(grid_.cell_size, segment.a.x, segment.b.x, x))
                {
                    // clang-format off
                    const auto y = column_border_intersection<rounding>(
                        grid_,
                        segment.a.x,
                        segment.a.y,
                        segment.b.x,
                        segment.b.y,
                        x);
                    // clang-format on
                    *out_it++ = { x, y };
                }
            }
        }
        if (segment.a.y != segment.b.y)
        {
            for (auto y = min_y; y <= max_y; y += tiles().tile_size())
            {
                if (border_between_coordinates(grid_.cell_size, segment.a.y, segment.b.y, y))
                {
                    // clang-format off
                    const auto x = row_border_intersection<rounding>(
                        grid_,
                        segment.a.x,
                        segment.a.y,
                        segment.b.x,
                        segment.b.y,
                        y);
                    // clang-format on
                    *out_it++ = { x, y };
                }
            }
        }
        return out_it;
    }

    /// @brief Checks line for intersection with the given cell.
    /// @param segment_on_line a segment defining a line through two points.
    /// @param cell the cell whose intersection needs to be checked.
    /// @returns True iff the line intersects the cell.
    [[nodiscard]] bool line_intersects_cell(const Segment2f64 & segment_on_line, const Vec2s64 & cell) const noexcept
    {
        return ka::line_intersects_cell<rounding>(
            grid_,
            segment_on_line.a.x,
            segment_on_line.a.y,
            segment_on_line.b.x,
            segment_on_line.b.y,
            cell.x,
            cell.y);
    }

public:
    [[nodiscard]] const GridParameters & grid() const noexcept
    {
        return grid_;
    }

    [[nodiscard]] const TileGrid & tiles() const noexcept
    {
        return tile_grid_;
    }

    [[nodiscard]] f64 cell_size() const noexcept
    {
        return grid_.cell_size;
    }

private:
    GridParameters grid_;
    TileGrid tile_grid_;
};

} // namespace ka
