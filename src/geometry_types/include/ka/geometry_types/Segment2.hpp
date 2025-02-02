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
};

using Segment2s64 = Segment2<s64>;
using Segment2u16 = Segment2<u16>;

} // namespace ka
