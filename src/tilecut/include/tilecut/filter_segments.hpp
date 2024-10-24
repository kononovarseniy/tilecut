#pragma once

#include <vector>

#include <geometry_types/Segment2.hpp>

namespace r7
{

/// @brief Removes all repeated and zero length segments.
void filter_segments(std::vector<Segment2s64> & segments) noexcept;

} // namespace r7
