#pragma once

namespace r7
{

/// @brief Rule for matching points to grid cells.
enum class GridRounding
{
    /// @brief Rounding down to the enclosing grid cell.
    Cell,
    /// @brief Rounding to the nearest grid node.
    NearestNode,
};

} // namespace r7
