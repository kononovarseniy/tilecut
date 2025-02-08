#pragma once

#include <algorithm>
#include <concepts>
#include <iterator>
#include <limits>
#include <span>
#include <vector>

#include <ka/common/assert.hpp>
#include <ka/common/fixed.hpp>
#include <ka/geometry_types/Vec2.hpp>
#include <ka/tilecut/HotPixelOrder.hpp>

namespace ka
{

//! A simple data structure for querying hot pixels within a given region.
//! Lifetime is bound to parent HotPixelCollector. The index is invalidated on HotPixelCollector modifications.
class HotPixelIndex final
{
    friend class HotPixelCollector;

public:
    /// @brief Retrieves all hot_pixels within the given rectangle.
    /// Pixels are returned in the order defined by ka::hot_pixel_less<horizontal_order, vertical_order>.
    template <HotPixelOrder horizontal_order, HotPixelOrder vertical_order, std::output_iterator<Vec2s64> Out>
    [[nodiscard]] Out find_if(
        const s64 min_x,
        const s64 max_x,
        const s64 min_y,
        const s64 max_y,
        Out output,
        std::predicate<Vec2s64> auto predicate) const
    {
        AR_PRE(min_x <= max_x);

        if constexpr (horizontal_order == HotPixelOrder::Ascending)
        {
            const auto first = std::lower_bound(
                columns_.cbegin(),
                columns_.cend(),
                min_x,
                [&](const auto & lhs, const auto & rhs_x)
                {
                    return lhs.x < rhs_x;
                });
            for (auto it = first; it != columns_.cend() && it->x <= max_x; ++it)
            {
                output = it->template find_if<vertical_order>(min_y, max_y, output, predicate);
            }
            return output;
        }
        else
        {
            const auto first = std::lower_bound(
                columns_.crbegin(),
                columns_.crend(),
                max_x,
                [&](const auto & lhs, const auto & rhs_x)
                {
                    return lhs.x > rhs_x;
                });
            for (auto it = first; it != columns_.crend() && it->x >= min_x; ++it)
            {
                output = it->template find_if<vertical_order>(min_y, max_y, output, predicate);
            }
            return output;
        }
    }

private:
    struct Column final
    {
        s64 x;
        std::span<const Vec2s64> pixels;

        template <HotPixelOrder vertical_order, std::output_iterator<Vec2s64> Out>
        [[nodiscard]] Out find_if(const s64 min_y, const s64 max_y, Out output, std::predicate<Vec2s64> auto predicate)
            const
        {
            AR_PRE(min_y <= max_y);
            if constexpr (vertical_order == HotPixelOrder::Ascending)
            {
                const auto first = std::lower_bound(
                    pixels.begin(),
                    pixels.end(),
                    min_y,
                    [&](const auto & lhs, const auto & rhs_y)
                    {
                        return lhs.y < rhs_y;
                    });
                for (auto it = first; it != pixels.end() && it->y <= max_y; ++it)
                {
                    if (predicate(*it))
                    {
                        *output++ = *it;
                    }
                }
            }
            else
            {
                const auto first = std::lower_bound(
                    pixels.rbegin(),
                    pixels.rend(),
                    max_y,
                    [&](const auto & lhs, const auto & rhs_y)
                    {
                        return lhs.y > rhs_y;
                    });
                for (auto it = first; it != pixels.rend() && it->y >= min_y; ++it)
                {
                    if (predicate(*it))
                    {
                        *output++ = *it;
                    }
                }
            }
            return output;
        }
    };

private:
    //! Contains x-ordered columns, each containing y-ordered hot pixels. Populated by HotPixelCollector.
    std::vector<Column> columns_;
};

} // namespace ka
