#pragma once

#include <common/fixed.hpp>
#include <exact/GridRounding.hpp>

namespace r7
{

/// @brief Checks line for intersection with the cell of regular grid.
/// @param a_x, a_y coordinates of the first point on the line.
/// @param b_x, b_y coordinates of the second point on the line.
/// @param size size of a grid cell.
/// @param c_x, c_y integer coordinates of the cell.
/// @param c_x, c_y integer coordinates of the cell.
/// @tparam rounding coordinates rounding mode.
/// @return true if the line intersects the cell.
template <GridRounding rounding>
[[nodiscard]] bool line_intersects_cell(f64 a_x, f64 a_y, f64 b_x, f64 b_y, f64 size, s64 c_x, s64 c_y) noexcept;

/// @brief Finds the column of a regular grid that contains the given coordinate.
/// @param x coordinate of the point.
/// @param size size of a grid cell.
/// @tparam rounding coordinates rounding mode.
/// @return Index of the fount column.
template <GridRounding rounding>
[[nodiscard]] s64 column_containing_position(f64 x, f64 size) noexcept;

/// @brief Same as column_containing_position but for rows.
template <GridRounding rounding>
[[nodiscard]] s64 row_containing_position(f64 y, f64 size) noexcept;

/// @brief Checks that the main boundary of a column or row lies between coordinates.
/// @return a <= size * n <= b.
[[nodiscard]] bool border_between_coordinates(f64 a, f64 b, f64 size, s64 x) noexcept;

/// @brief Finds the row containing the intersection point
/// of a given line and the left border of a regular grid column.
/// @param a_x, a_y coordinates of the first point on the line.
/// @param b_x, b_y coordinates of the second point on the line.
/// @param size size of a grid cell.
/// @param c_x integer X coordinate of the column.
/// @tparam rounding coordinates rounding mode.
/// @return The Y coordinate of the found row.
template <GridRounding rounding>
[[nodiscard]] s64 column_border_intersection(f64 a_x, f64 a_y, f64 b_x, f64 b_y, f64 size, s64 c_x) noexcept;

/// @brief Finds the column containing the intersection point
/// of a given line and the bottom border of a regular grid row.
/// @param a_x, a_y coordinates of the first point on the line.
/// @param b_x, b_y coordinates of the second point on the line.
/// @param size size of a grid cell.
/// @param c_y integer Y coordinate of the row.
/// @tparam rounding coordinates rounding mode.
/// @return The X coordinate of the found column.
template <GridRounding rounding>
[[nodiscard]] s64 row_border_intersection(f64 a_x, f64 a_y, f64 b_x, f64 b_y, f64 size, s64 c_y) noexcept;

} // namespace r7
