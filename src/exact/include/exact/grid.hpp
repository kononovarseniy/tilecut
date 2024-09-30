#pragma once

#include <common/fixed.hpp>

namespace r7
{

constexpr f64 g_max_regular_grid_input_coordinate = 0x1.0p-25;

/// @brief Checks line for intersection with the cell of regular grid.
/// @param a_x, a_y coordinates of the first point on the line.
/// @param b_x, b_y coordinates of the second point on the line.
/// @param size size of a grid cell.
/// @param c_x, c_y integer coordinates of the cell.
/// @return true if the line intersects the cell.
[[nodiscard]] bool line_intersects_cell(
    const f64 a_x,
    const f64 a_y,
    const f64 b_x,
    const f64 b_y,
    const f64 size,
    const s64 c_x,
    const s64 c_y) noexcept;

/// @brief Finds the column of a regular grid that contains the given coordinate.
/// @param x coordinate of the point.
/// @param size size of a grid cell.
/// @return Index of the fount column.
[[nodiscard]] s64 column_containing_position(const f64 x, const f64 size) noexcept;

/// @brief Finds the row containing the intersection point
/// of a given line and the left border of a regular grid column.
/// @param a_x, a_y coordinates of the first point on the line.
/// @param b_x, b_y coordinates of the second point on the line.
/// @param size size of a grid cell.
/// @param c_x integer X coordinate of the column.
/// @return The Y coordinate of the found row.
[[nodiscard]] s64 column_border_intersecion(
    const f64 a_x,
    const f64 a_y,
    const f64 b_x,
    const f64 b_y,
    const f64 size,
    const s64 c_x) noexcept;

} // namespace r7
