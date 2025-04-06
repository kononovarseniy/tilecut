#pragma once

#include <ka/common/fixed.hpp>
#include <ka/exact/orientation.hpp>
#include <ka/geometry_types/Segment2.hpp>
#include <ka/geometry_types/Vec2.hpp>

#define KA_TILECUT_ORIENTATION_CLASSIFIER(NAME, GT, GE, EQ, LE, LT)                                                    \
    template <typename T>                                                                                              \
    class NAME final                                                                                                   \
    {                                                                                                                  \
    public:                                                                                                            \
        constexpr NAME(const T value) noexcept                                                                         \
            : value_ { value }                                                                                         \
        {                                                                                                              \
        }                                                                                                              \
                                                                                                                       \
        [[nodiscard]] constexpr bool GT() const noexcept                                                               \
        {                                                                                                              \
            return value_ > 0;                                                                                         \
        }                                                                                                              \
                                                                                                                       \
        [[nodiscard]] constexpr bool GE() const noexcept                                                               \
        {                                                                                                              \
            return value_ >= 0;                                                                                        \
        }                                                                                                              \
                                                                                                                       \
        [[nodiscard]] constexpr bool EQ() const noexcept                                                               \
        {                                                                                                              \
            return value_ == 0;                                                                                        \
        }                                                                                                              \
                                                                                                                       \
        [[nodiscard]] constexpr bool LE() const noexcept                                                               \
        {                                                                                                              \
            return value_ <= 0;                                                                                        \
        }                                                                                                              \
                                                                                                                       \
        [[nodiscard]] constexpr bool LT() const noexcept                                                               \
        {                                                                                                              \
            return value_ < 0;                                                                                         \
        }                                                                                                              \
                                                                                                                       \
        [[nodiscard]] constexpr bool operator==(const NAME & other) const noexcept                                     \
        {                                                                                                              \
            return (value_ <=> 0) == (other.value_ <=> 0);                                                             \
        }                                                                                                              \
                                                                                                                       \
    private:                                                                                                           \
        T value_;                                                                                                      \
    };

namespace ka
{

KA_TILECUT_ORIENTATION_CLASSIFIER(PointLocationClassifier, left, left_or_line, line, right_or_line, right);

/// @brief Determines the location of a point relative to a given line.
template <typename T>
[[nodiscard]] constexpr auto point_location(const Vec2<T> line_a, const Vec2<T> line_b, const Vec2<T> point_c) noexcept
{
    return PointLocationClassifier { orientation(line_a.x, line_a.y, line_b.x, line_b.y, point_c.x, point_c.y) };
}

/// @brief Determines the location of a point relative to a line represented be the segment.
template <typename T>
[[nodiscard]] constexpr auto point_location(const Segment2<T> segment, const Vec2<T> point) noexcept
{
    return point_location(segment.a, segment.b, point);
}

KA_TILECUT_ORIENTATION_CLASSIFIER(
    PointOrderClassifier,
    is_ccw,
    is_ccw_or_collinear,
    is_collinear,
    is_cw_or_collinear,
    is_cw);

/// @brief Determines the winding order of three points.
template <typename T>
[[nodiscard]] constexpr auto point_order(const Vec2<T> a, const Vec2<T> b, const Vec2<T> c) noexcept
{
    return PointOrderClassifier { orientation(a.x, a.y, b.x, b.y, c.x, c.y) };
}

KA_TILECUT_ORIENTATION_CLASSIFIER(
    VertexTypeClassifier,
    is_convex,
    is_convex_or_straight,
    is_straight,
    is_reflex_or_straight,
    is_reflex);

/// @brief Determines vertex type (convex, reflex, or collinear)
/// by analyzing the winding order of three consecutive points.
template <typename T>
[[nodiscard]] constexpr auto vertex_type(const Vec2<T> prev, const Vec2<T> curr, const Vec2<T> next) noexcept
{
    return VertexTypeClassifier { orientation(prev.x, prev.y, curr.x, curr.y, next.x, next.y) };
}

} // namespace ka