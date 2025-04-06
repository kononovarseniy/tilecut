#pragma once

#include <source_location>

#include <ka/common/cast.hpp>
#include <ka/common/fixed.hpp>
#include <ka/common/hash.hpp>
#include <ka/geometry_types/Vec2.hpp>

namespace ka
{

template <typename T>
struct Segment2 final
{
    using CoordinateType = T;

    Vec2<T> a;
    Vec2<T> b;

    void hash(Hasher & hasher) const noexcept
    {
        hasher.update(a);
        hasher.update(b);
    }

    [[nodiscard]] constexpr auto operator<=>(const Segment2<T> &) const noexcept = default;

    template <typename Target>
    constexpr Segment2<Target> exact_cast(
        const std::source_location & location = std::source_location::current()) const noexcept
    {
        return {
            .a = a.template exact_cast<Target>(location),
            .b = b.template exact_cast<Target>(location),
        };
    }

    /// @brief Checks if the segment is degenerate (its endpoints are identical),
    [[nodiscard]] constexpr bool degenerate() const noexcept
    {
        return a == b;
    }

    /// @brief Creates a canonical representation for undirected segment comparison.
    /// @return Copy of the segment where start point `a` is less than or equal to end point `b`.
    [[nodiscard]] constexpr auto to_undirected() const noexcept
    {
        if (a > b)
        {
            return Segment2 { b, a };
        }
        return *this;
    }
};

template <typename T>
concept IsSegment2 = std::same_as<T, Segment2<typename T::CoordinateType>>;

using Segment2u8 = Segment2<u8>;
using Segment2u16 = Segment2<u16>;
using Segment2u32 = Segment2<u32>;
using Segment2u64 = Segment2<u64>;
using Segment2s8 = Segment2<s8>;
using Segment2s16 = Segment2<s16>;
using Segment2s32 = Segment2<s32>;
using Segment2s64 = Segment2<s64>;
using Segment2f32 = Segment2<f32>;
using Segment2f64 = Segment2<f64>;

} // namespace ka
