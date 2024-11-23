#pragma once

#include <vector>

#include <geometry_types/Segment2.hpp>

namespace ka
{

/// @brief Removes all repeated and zero length segments.
void filter_segments(std::vector<Segment2s64> & segments) noexcept;

} // namespace ka
