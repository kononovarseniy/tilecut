/// Polygon orientation is often determined by calculating its oriented (signed) area. However, this method suffers from
/// numerical precision limitations. For floating-point numbers, error accumulation rapidly distorts the area's sign,
/// while integer computations frequently cause overflow issues.
///
/// To determine orientation, we establish whether the half-plane to the polygon's right lies inside or outside the
/// polygon. We construct this half-plane using the rightmost vertex of the multipolygon. Since the vertex is the
/// rightmost, all segments and vertices of the polygon can be either to the left or on the boundary of the half-plane.
/// Therefore, the half-plane is either entirely inside the multipolygon or entirely outside. We select the topmost
/// point on this boundary and find the edge oriented furthest clockwise relative to the upward direction. This requires
/// an exact orientation predicate for three points. As we operate in the left half-plane, collinear segments may either
/// share interior points (prohibited by data constraints) or be vertically counter-directed. By choosing a point with
/// no strictly higher neighbors, such counter-directed edges are impossible. Zero-length edges are prohibited in input
/// data, as they are better handled by the caller if needed.
///
/// The selected edge will separate the polygon's interior and exterior regions due to the prohibition of overlapping
/// edges. Within a neighborhood of the chosen vertex, a continuous path exists from the edge to the analyzed
/// half-plane. If the selected edge enters the chosen vertex, the half-plane lies outside, and the outer contour is
/// oriented counter-clockwise. If the edge exits the vertex, the half-plane is inside, and the polygon is
/// clockwise-oriented.
///
/// Alternatively, one can eliminate Y-coordinate ordering. This requires several adjustments. First, filter edges by
/// comparing both endpoints with the selected point, as reordering points within a segment doesn’t guarantee its second
/// point matches the selection. Second, when sorting edges clockwise, properly select vertices distinct from the chosen
/// one. Since the selected point might not be the absolute top/bottom on the half-plane boundary, collinear edges may
/// occur. Their ordering under collinearity (per predicate) requires Y-coordinate comparison. This approach increases
/// implementation complexity, hence we use the previously described X-then-Y ordering.

#pragma once

#include <algorithm>
#include <optional>
#include <ranges>

#include <ka/common/assert.hpp>
#include <ka/geometry_types/Segment2.hpp>
#include <ka/geometry_types/Vec2.hpp>
#include <ka/tilecut/orient.hpp>

namespace ka
{
namespace detail
{

template <typename T>
class PolygonOrientationMainSegmentChooser;

} // namespace detail

/// @brief The orientation of the polygon having non-zero area.
enum class PolygonOrientation : bool
{
    Clockwise = false,
    CounterClockwise = true,
};

/// @brief Determines the orientation of the closed contour.
/// @pre The contour is closed. The contour has no self-intersections or repeated points. Any two segments cannot have
/// common points except shared endpoints.
/// @note The contour is allowed to touch itself at the vertices.
template <std::ranges::forward_range R>
    requires IsVec2<std::ranges::range_value_t<R>>
[[nodiscard]] PolygonOrientation contour_orientation(const R & contour)
{
    AR_PRE(std::ranges::size(contour) >= 3);
    AR_PRE(std::ranges::adjacent_find(contour) == std::ranges::end(contour));

    const auto max_vertex = std::ranges::max(contour);

    detail::PolygonOrientationMainSegmentChooser segment_chooser { max_vertex };
    for (auto prev_it = std::ranges::begin(contour), curr_it = std::ranges::next(prev_it);
         curr_it != std::ranges::end(contour);
         prev_it = curr_it++)
    {
        segment_chooser.process_segment({ *prev_it, *curr_it });
    }
    return static_cast<PolygonOrientation>(segment_chooser.chosen_segment().b == max_vertex);
}

/// @brief Determines the orientation of the outermost contour of the contour set represented by the given segment set.
/// Orientations of interior contours are ignored. When there are several outermost contours, one is arbitrarily chosen.
/// @pre Segments form a set of closed contours. These contours have no intersections, self-intersections, or repeated
/// points. Any two segments cannot have common points except shared endpoints.
/// @note Contours are allowed to touch themselves and other contours only at common vertices.
template <std::ranges::forward_range R>
    requires IsSegment2<std::ranges::range_value_t<R>>
[[nodiscard]] PolygonOrientation polygon_orientation(const R & segments)
{
    using SegmentType = std::ranges::range_value_t<R>;

    AR_PRE(std::ranges::size(segments) >= 3);
    AR_PRE(std::ranges::none_of(segments, &SegmentType::degenerate));

    const auto max_vertex = std::ranges::max(
        segments |
        std::views::transform(
            [](const auto & segment)
            {
                return std::max(segment.a, segment.b);
            }));

    detail::PolygonOrientationMainSegmentChooser segment_chooser { max_vertex };
    for (const auto & segment : segments)
    {
        segment_chooser.process_segment(segment);
    }
    return static_cast<PolygonOrientation>(segment_chooser.chosen_segment().b == max_vertex);
}

namespace detail
{

// TODO: [C++23] Use std::pairwise_transform and generic algorithm.
template <typename T>
class PolygonOrientationMainSegmentChooser final
{
public:
    PolygonOrientationMainSegmentChooser(const Vec2<T> & max_vertex)
        : max_vertex_ { max_vertex }
        , is_first_ { true }
    {
    }

    void process_segment(const Segment2<T> & segment)
    {
        const auto undirected = segment.to_undirected();
        if (undirected.b != max_vertex_)
        {
            return;
        }
        AR_PRE(is_first_ || max_undirected_.a != undirected.a);
        if (is_first_ || point_location(max_undirected_.a, max_vertex_, undirected.a).right())
        {
            is_first_ = false;
            max_segment_ = segment;
            max_undirected_ = undirected;
        }
    }

    [[nodiscard]] const Segment2<T> & chosen_segment() const noexcept
    {
        AR_PRE(!is_first_);
        return max_segment_;
    }

private:
    Vec2<T> max_vertex_;
    bool is_first_;
    Segment2<T> max_segment_;
    Segment2<T> max_undirected_;
};

} // namespace detail

} // namespace ka
