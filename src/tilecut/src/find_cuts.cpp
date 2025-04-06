#include <algorithm>
#include <optional>
#include <utility>

#include <ka/common/assert.hpp>
#include <ka/common/cast.hpp>
#include <ka/common/fixed.hpp>
#include <ka/geometry_types/Vec2.hpp>
#include <ka/tilecut/find_cuts.hpp>
#include <ka/tilecut/orient.hpp>

namespace ka
{

inline namespace
{

/// @brief Converts point to coresponding parameter value.
/// @return The point parameter, that is, the distance along the perimeter (counter-clockwise) of the tile from the
/// corner with zero coordinates to the point.
[[nodiscard]] std::optional<u32> make_parameter(const u16 tile_size, const Vec2u16 point) noexcept
{
    if (point.y == 0)
    {
        return point.x;
    }
    if (point.x == tile_size)
    {
        return tile_size + point.y;
    }
    if (point.y == tile_size)
    {
        return tile_size * 2 + (tile_size - point.x);
    }
    if (point.x == 0)
    {
        return tile_size * 3 + (tile_size - point.y);
    }
    return std::nullopt;
}

/// @brief Converts parameter value to coresponding point.
/// @return Point on the tile boundary.
[[nodiscard]] Vec2u16 make_point(const u16 tile_size, const u32 parameter) noexcept
{
    const auto side_parameter = exact_cast<u16>(parameter % tile_size);
    switch (parameter / tile_size % 4)
    {
    case 0:
        return { side_parameter, 0 };
    case 1:
        return { tile_size, side_parameter };
    case 2:
        return { exact_cast<u16>(tile_size - side_parameter), tile_size };
    case 3:
        return { 0, exact_cast<u16>(tile_size - side_parameter) };
    default:
        AR_UNREACHABLE;
    }
}

/// @brief Adds tile cut segments along the perimeter of the tile from the point defined by the from_parameter value to
/// the point defined by the to_parameter value.
/// @param tile_size size of the tile.
/// @param result output container.
/// @param from_parameter parameter value of the first point.
/// @param to_parameter parameter value of the second point.
void add_cut(
    const u16 tile_size,
    std::vector<Segment2u16> & result,
    const u32 from_parameter,
    const u32 to_parameter) noexcept
{
    AR_PRE(from_parameter < to_parameter);

    auto prev = make_point(tile_size, from_parameter);
    for (auto corner_parameter = (from_parameter / tile_size + 1) * tile_size; corner_parameter < to_parameter;
         corner_parameter += tile_size)
    {
        const auto corner = make_point(tile_size, corner_parameter);
        AR_POST(prev != corner);
        result.push_back({ prev, corner });
        AR_POST(
            (prev.x == corner.x && (prev.x == 0 || prev.x == tile_size)) ||
            (prev.y == corner.y && (prev.y == 0 || prev.y == tile_size)));
        prev = corner;
    }
    const auto end = make_point(tile_size, to_parameter);
    result.push_back({ prev, end });
    AR_POST(prev != end);
    AR_POST(
        (prev.x == end.x && (prev.x == 0 || prev.x == tile_size)) ||
        (prev.y == end.y && (prev.y == 0 || prev.y == tile_size)));
}

/// @brief Checks that all maximum inclusion contours are oriented counter-clockwise.
/// @param segments a collection of non-intersecting oriented segments, none of which touches the boundary of a tile,
/// that form a set of contours.
[[nodiscard]] bool outermost_contour_is_inner(const std::span<const Segment2u16> segments) noexcept
{
    AR_PRE(!segments.empty());
    const auto segment_it = std::ranges::min_element(
        segments,
        [](const auto & lhs, const auto & rhs) noexcept
        {
            if (lhs.b == rhs.b)
            {
                return point_order(lhs.b, lhs.a, rhs.a).is_cw();
            }
            return lhs.b > rhs.b;
        },
        [](const Segment2u16 & segment) noexcept -> Segment2u16
        {
            return { std::min(segment.a, segment.b), std::max(segment.a, segment.b) };
        });
    AR_ASSERT(segment_it != segments.end());
    return segment_it->a > segment_it->b;
}

struct TouchingSegment final
{
    /// Parameter of the touching_point.
    u32 parameter;
    /// Position of the point on the tile boundary.
    Vec2u16 touching_point;
    /// Position of the point opposite to one located on the tile boundary.
    Vec2u16 opposite_point;
    /// If true, the second point of the original segment is located on the tile boundary.
    /// This means that some part of the boundary clockwise from the touch point is to the right of the segment,
    /// or that the segment lies entirely on the boundary.
    /// For many simple geometries, this means that the next segment of the contour belongs to a different tile,
    /// hence the name
    bool outgoing;
};

/// @brief Checks the precondition of the orientation of segments on the boundary.
[[nodiscard]] [[maybe_unused]] bool check_orientation_if_on_boundary(
    const u16 tile_size,
    const TouchingSegment & touching_segment) noexcept
{
    auto a = touching_segment.touching_point;
    auto b = touching_segment.opposite_point;
    AR_PRE(a != b);
    if (touching_segment.outgoing)
    {
        std::swap(a, b);
    }
    if (a.x == 0 && b.x == 0 && a.y < b.y)
    {
        return false;
    }
    if (a.x == tile_size && b.x == tile_size && a.y > b.y)
    {
        return false;
    }
    if (a.y == 0 && b.y == 0 && a.x > b.x)
    {
        return false;
    }
    if (a.y == tile_size && b.y == tile_size && a.x < b.x)
    {
        return false;
    }
    return true;
}

} // namespace

void find_cuts(
    const TileGrid & tile_grid,
    const std::span<const Segment2u16> segments,
    std::vector<Segment2u16> & result) noexcept
{
    if (segments.empty())
    {
        return;
    }
    // TODO: Reuse vector.
    std::vector<TouchingSegment> touching_segments;
    touching_segments.reserve(segments.size() * 2);
    for (const auto & segment : segments)
    {
        const auto begin_param = make_parameter(tile_grid.tile_size(), segment.a);
        const auto end_param = make_parameter(tile_grid.tile_size(), segment.b);

        if (begin_param.has_value())
        {
            touching_segments.push_back({ *begin_param, segment.a, segment.b, false });
        }
        if (end_param.has_value())
        {
            touching_segments.push_back({ *end_param, segment.b, segment.a, true });
        }
    }

    AR_ASSERT(touching_segments.size() % 2 == 0);

    // A special case is when no segment touches the boundary.
    // We check the orientation of the contours to see if the polygon contains the entire tile boundary.
    if (touching_segments.empty())
    {
        if (outermost_contour_is_inner(segments))
        {
            add_cut(tile_grid.tile_size(), result, 0, tile_grid.tile_size() * 4);
        }
    }
    else
    {
        // Sort touching segments counter-clockwise by boundary_point then clockwise by opposite_point.
        // The direction of the first segment in each bunch (group of sergments with the same touching_point)
        // determines whether the boundary section from the bunch to the previous one
        // lies inside the polygon or outside it.
        std::ranges::sort(
            touching_segments,
            [&](const auto & lhs, const auto & rhs) noexcept
            {
                if (lhs.parameter != rhs.parameter)
                {
                    return lhs.parameter < rhs.parameter;
                }
                AR_ASSERT(lhs.touching_point == rhs.touching_point);
                AR_ASSERT(lhs.opposite_point != rhs.opposite_point);
                const auto order = point_order(lhs.touching_point, lhs.opposite_point, rhs.opposite_point);
                // If both points lie on the same side of the tile boundary, the orientation check is insufficient.
                // The most counter-clockwise segment is the segment with the smaller parameter of the opposite point
                if (order.is_collinear())
                {
                    const auto lhs_param = make_parameter(tile_grid.tile_size(), lhs.opposite_point);
                    const auto rhs_param = make_parameter(tile_grid.tile_size(), rhs.opposite_point);
                    AR_ASSERT(lhs_param.has_value() && rhs_param.has_value());
                    // A very special case of collinear opposite points, one of which is zero. For such a point, the
                    // parameter value is ambiguous. This is only possible if either x or y is zero for all points. For
                    // the boundary y = 0, the most counterclockwise segment is the segment with the zero opposite
                    // point, and vice versa for the boundary x = 0.
                    if (lhs_param == 0u)
                    {
                        AR_ASSERT(rhs_param != 0u);
                        return lhs.touching_point.y == 0;
                    }
                    if (rhs_param == 0u)
                    {
                        AR_ASSERT(lhs_param != 0u);
                        return lhs.touching_point.y != 0;
                    }
                    return lhs_param < rhs_param;
                }
                return order.is_cw();
            });

        std::optional<u32> prev_point;
        const auto process_bunch = [&](const TouchingSegment & cw_segment, const bool repeated_first)
        {
            AR_PRE(check_orientation_if_on_boundary(tile_grid.tile_size(), cw_segment));
            /// The most clockwise segment of the bunch determines whether the previous part of the boundary
            /// belongs to the multipolygon.
            /// When the segment is not on the boundary, the previous part of the boundary is to the right of it and
            /// therefore does not belong to the polygon.
            /// When the segment is on the boundary, the precondition above ensures that the outgoing segment lies on
            /// the previous part of the boundary (which is not a cut since it coincides with the existing segment),
            /// and the non-outgoing segment lies on the unprocessed part of the boundary.
            const auto previous_boundary_part_is_cut = !cw_segment.outgoing;
            if (previous_boundary_part_is_cut)
            {
                if (prev_point.has_value())
                {
                    add_cut(
                        tile_grid.tile_size(),
                        result,
                        *prev_point,
                        repeated_first ? tile_grid.tile_size() * 4 + cw_segment.parameter : cw_segment.parameter);
                }
                else
                {
                    AR_ASSERT(!repeated_first);
                }
            }

            prev_point = cw_segment.parameter;
        };

        for (auto it = touching_segments.begin(); it != touching_segments.end();
             it = std::find_if(
                 it,
                 touching_segments.end(),
                 [&](const auto & segment)
                 {
                     return segment.parameter != it->parameter;
                 }))
        {
            process_bunch(*it, false);
        }
        process_bunch(*touching_segments.begin(), true);
    }
}

bool open_on_the_bottom(const std::span<const Segment2u16> cut_segments) noexcept
{
    return std::ranges::any_of(
        cut_segments,
        [&](const Segment2u16 & segment)
        {
            return segment.a.y == 0 && segment.b.y == 0;
        });
}

} // namespace ka
