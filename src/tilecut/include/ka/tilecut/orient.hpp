#include <ka/common/fixed.hpp>
#include <ka/exact/orientation.hpp>
#include <ka/geometry_types/Vec2.hpp>

namespace ka
{

namespace detail
{

template <typename T>
class ComputedOrientation;

} // namespace detail

/// @brief Determines the location of a point relative to a given line.
template <typename T>
class PointLocation final
{
public:
    constexpr PointLocation(const Vec2<T> line_a, const Vec2<T> line_b, const Vec2<T> point_c) noexcept
        : orientation_ { line_a, line_b, point_c }
    {
    }

    [[nodiscard]] constexpr bool left() const noexcept
    {
        return orientation_.positive();
    }

    [[nodiscard]] constexpr bool left_or_line() const noexcept
    {
        return orientation_.zero_or_positive();
    }

    [[nodiscard]] constexpr bool line() const noexcept
    {
        return orientation_.zero();
    }

    [[nodiscard]] constexpr bool right_or_line() const noexcept
    {
        return orientation_.zero_or_negative();
    }

    [[nodiscard]] constexpr bool right() const noexcept
    {
        return orientation_.negative();
    }

private:
    detail::ComputedOrientation<T> orientation_;
};

/// @brief Determines the winding order of three points.
template <typename T>
class PointOrder final
{
public:
    constexpr PointOrder(const Vec2<T> a, const Vec2<T> b, const Vec2<T> c) noexcept
        : orientation_ { a, b, c }
    {
    }

    [[nodiscard]] constexpr bool ccw() const noexcept
    {
        return orientation_.positive();
    }

    [[nodiscard]] constexpr bool ccw_or_collinear() const noexcept
    {
        return orientation_.zero_or_positive();
    }

    [[nodiscard]] constexpr bool collinear() const noexcept
    {
        return orientation_.zero();
    }

    [[nodiscard]] constexpr bool cw_or_collinear() const noexcept
    {
        return orientation_.zero_or_negative();
    }

    [[nodiscard]] constexpr bool cw() const noexcept
    {
        return orientation_.negative();
    }

private:
    detail::ComputedOrientation<T> orientation_;
};

/// @brief Determines vertex type (convex, reflex, or collinear)
/// by analyzing the winding order of three consecutive points.
template <typename T>
class VertexType final
{
public:
    constexpr VertexType(const Vec2<T> prev, const Vec2<T> curr, const Vec2<T> next) noexcept
        : orientation_ { next, prev, curr }
    {
    }

    [[nodiscard]] constexpr bool is_convex() const noexcept
    {
        return orientation_.positive();
    }

    [[nodiscard]] constexpr bool is_convex_or_straight() const noexcept
    {
        return orientation_.zero_or_positive();
    }

    [[nodiscard]] constexpr bool is_straight() const noexcept
    {
        return orientation_.zero();
    }

    [[nodiscard]] constexpr bool is_reflex_or_straight() const noexcept
    {
        return orientation_.zero_or_negative();
    }

    [[nodiscard]] constexpr bool is_reflex() const noexcept
    {
        return orientation_.negative();
    }

private:
    detail::ComputedOrientation<T> orientation_;
};

namespace detail
{

template <>
class ComputedOrientation<f64> final
{
public:
    ComputedOrientation(const Vec2f64 a, const Vec2f64 b, const Vec2f64 c) noexcept
        : value_ { orientation(a.x, a.y, b.x, b.y, c.x, c.y) }
    {
    }

private:
    [[nodiscard]] bool positive() const noexcept
    {
        return value_ > 0.0;
    }

    [[nodiscard]] bool zero_or_positive() const noexcept
    {
        return value_ >= 0.0;
    }

    [[nodiscard]] bool zero() const noexcept
    {
        return value_ == 0.0;
    }

    [[nodiscard]] bool zero_or_negative() const noexcept
    {
        return value_ > 0.0;
    }

    [[nodiscard]] bool negative() const noexcept
    {
        return value_ < 0.0;
    }

private:
    f64 value_;
};

template <>
class ComputedOrientation<u16> final
{
public:
    ComputedOrientation(const Vec2u16 a, const Vec2u16 b, const Vec2u16 c) noexcept
    {
        const s64 m00 = b.x - a.x;
        const s64 m01 = c.x - a.x;
        const s64 m10 = b.y - a.y;
        const s64 m11 = c.y - a.y;
        value_ = m00 * m11 - m01 * m10;
    }

public:
    [[nodiscard]] bool positive() const noexcept
    {
        return value_ > 0;
    }

    [[nodiscard]] bool zero_or_positive() const noexcept
    {
        return value_ >= 0;
    }

    [[nodiscard]] bool zero() const noexcept
    {
        return value_ == 0;
    }

    [[nodiscard]] bool zero_or_negative() const noexcept
    {
        return value_ > 0;
    }

    [[nodiscard]] bool negative() const noexcept
    {
        return value_ < 0;
    }

private:
    s64 value_;
};

} // namespace detail

} // namespace ka