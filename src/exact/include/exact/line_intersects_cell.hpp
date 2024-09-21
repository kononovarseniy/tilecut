#pragma once

#include <common/fixed.hpp>

namespace r7
{

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

} // namespace r7
