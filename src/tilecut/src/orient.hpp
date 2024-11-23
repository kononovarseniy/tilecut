#include <common/fixed.hpp>
#include <geometry_types/Vec2.hpp>

namespace ka
{

[[nodiscard]] constexpr s64 orient(const Vec2u16 line_a, const Vec2u16 line_b, const Vec2u16 point_c) noexcept
{
    const s64 m00 = line_b.x - line_a.x;
    const s64 m01 = point_c.x - line_a.x;
    const s64 m10 = line_b.y - line_a.y;
    const s64 m11 = point_c.y - line_a.y;
    return m00 * m11 - m01 * m10;
}

class PointLocation final
{
public:
    constexpr PointLocation(const Vec2u16 line_a, const Vec2u16 line_b, const Vec2u16 point_c) noexcept
        : value_ { orient(line_a, line_b, point_c) }
    {
    }

    [[nodiscard]] constexpr bool left() const noexcept
    {
        return value_ > 0;
    }

    [[nodiscard]] constexpr bool left_or_line() const noexcept
    {
        return value_ >= 0;
    }

    [[nodiscard]] constexpr bool line() const noexcept
    {
        return value_ == 0;
    }

    [[nodiscard]] constexpr bool right_or_line() const noexcept
    {
        return value_ <= 0;
    }

    [[nodiscard]] constexpr bool right() const noexcept
    {
        return value_ < 0;
    }

private:
    s64 value_;
};

class PointOrder final
{
public:
    constexpr PointOrder(const Vec2u16 a, const Vec2u16 b, const Vec2u16 c) noexcept
        : value_ { orient(a, b, c) }
    {
    }

    [[nodiscard]] constexpr bool ccw() const noexcept
    {
        return value_ > 0;
    }

    [[nodiscard]] constexpr bool ccw_or_collinear() const noexcept
    {
        return value_ >= 0;
    }

    [[nodiscard]] constexpr bool collinear() const noexcept
    {
        return value_ == 0;
    }

    [[nodiscard]] constexpr bool cw_or_collinear() const noexcept
    {
        return value_ <= 0;
    }

    [[nodiscard]] constexpr bool cw() const noexcept
    {
        return value_ < 0;
    }

private:
    s64 value_;
};

class VertexType final
{
public:
    constexpr VertexType(const Vec2u16 prev, const Vec2u16 curr, const Vec2u16 next) noexcept
        : value_ { orient(next, prev, curr) }
    {
    }

    [[nodiscard]] constexpr bool is_convex() const noexcept
    {
        return value_ > 0;
    }

    [[nodiscard]] constexpr bool is_convex_or_straight() const noexcept
    {
        return value_ >= 0;
    }

    [[nodiscard]] constexpr bool is_straight() const noexcept
    {
        return value_ == 0;
    }

    [[nodiscard]] constexpr bool is_reflex_or_straight() const noexcept
    {
        return value_ <= 0;
    }

    [[nodiscard]] constexpr bool is_reflex() const noexcept
    {
        return value_ < 0;
    }

private:
    s64 value_;
};

} // namespace ka