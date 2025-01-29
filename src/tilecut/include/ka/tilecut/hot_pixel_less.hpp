#pragma once

#include <ka/geometry_types/Vec2.hpp>
#include <ka/tilecut/HotPixelOrder.hpp>

namespace ka
{

template <HotPixelOrder x_order, HotPixelOrder y_order>
struct hot_pixel_less final
{
    constexpr bool operator()(const Vec2s64 lhs, const Vec2s64 rhs) noexcept
    {
        if (lhs.x != rhs.x)
        {
            if constexpr (x_order == HotPixelOrder::Ascending)
            {
                return lhs.x < rhs.x;
            }
            else
            {
                return lhs.x > rhs.x;
            }
        }
        if constexpr (y_order == HotPixelOrder::Ascending)
        {
            return lhs.y < rhs.y;
        }
        else
        {
            return lhs.y > rhs.y;
        }
    }
};

} // namespace ka
