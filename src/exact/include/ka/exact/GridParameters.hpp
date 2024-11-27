#pragma once

#include <ka/common/fixed.hpp>

namespace ka
{

/// @brief Parameters of the grid and related exactly precomputed constants used in exact computations.
/// Narrowing allowed range of input coordinates and/or increasing the cell size
/// is correct even without recalculating constants.
struct GridParameters final
{
    /// @brief Size of a grid cell.
    f64 cell_size;

    /// @brief The size of the grid cell that was used to calculate the parameters.
    f64 desired_cell_size;

    /// @brief Minimal allowed absolute value of input coordinates.
    f64 min_input;

    /// @brief Maximal allowed absolute value of input coordinates.
    f64 max_input;

    struct
    {
        /// @brief Smaller values of the fractional part do not guarantee correct rounding in the
        /// column_border_intersecion.
        f64 min_reliable_fractional_part;

        /// @brief Larger values of the fractional part do not guarantee correct rounding in the
        /// column_border_intersecion.
        f64 max_reliable_fractional_part;
    } column_border_intersecion;
};

} // namespace ka
