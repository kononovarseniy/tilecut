#include <tilecut/uvec16.h>

#include <common/fixed.hpp>


namespace ka::tilecut
{

[[nodiscard]] constexpr r7::s64 orient(const uvec16 line_a, const uvec16 line_b, const uvec16 point_c) noexcept
{
    const r7::s64 m00 = line_b.x - line_a.x;
    const r7::s64 m01 = point_c.x - line_a.x;
    const r7::s64 m10 = line_b.y - line_a.y;
    const r7::s64 m11 = point_c.y - line_a.y;
    return m00 * m11 - m01 * m10;
}

class PointLocation final
{
public:
    constexpr PointLocation(const uvec16 line_a, const uvec16 line_b, const uvec16 point_c) noexcept
        : value_ { orient(line_a, line_b, point_c) }
    {
    }

public:
    [[nodiscard]] constexpr bool left() const
    {
        return value_ > 0;
    }

    [[nodiscard]] constexpr bool left_or_line() const
    {
        return value_ >= 0;
    }

    [[nodiscard]] constexpr bool line() const
    {
        return value_ == 0;
    }

    [[nodiscard]] constexpr bool right_or_line() const
    {
        return value_ <= 0;
    }

    [[nodiscard]] constexpr bool right() const
    {
        return value_ < 0;
    }

private:
    r7::s64 value_;
};

class PointOrder final
{
public:
    constexpr PointOrder(const uvec16 a, const uvec16 b, const uvec16 c) noexcept
        : value_ { orient(a, b, c) }
    {
    }

public:
    [[nodiscard]] constexpr bool ccw() const
    {
        return value_ > 0;
    }

    [[nodiscard]] constexpr bool ccw_or_colinear() const
    {
        return value_ >= 0;
    }

    [[nodiscard]] constexpr bool colinear() const
    {
        return value_ == 0;
    }

    [[nodiscard]] constexpr bool cw_or_colinear() const
    {
        return value_ <= 0;
    }

    [[nodiscard]] constexpr bool cw() const
    {
        return value_ < 0;
    }

private:
    r7::s64 value_;
};

class VertexType final
{
public:
    constexpr VertexType(const uvec16 prev, const uvec16 curr, const uvec16 next) noexcept
        : value_ { orient(next, prev, curr) }
    {
    }

public:
    [[nodiscard]] constexpr bool is_convex() const
    {
        return value_ > 0;
    }

    [[nodiscard]] constexpr bool is_convex_or_straight() const
    {
        return value_ >= 0;
    }

    [[nodiscard]] constexpr bool is_straight() const
    {
        return value_ == 0;
    }

    [[nodiscard]] constexpr bool is_reflex_or_straight() const
    {
        return value_ <= 0;
    }

    [[nodiscard]] constexpr bool is_reflex() const
    {
        return value_ < 0;
    }

private:
    r7::s64 value_;
};

}
