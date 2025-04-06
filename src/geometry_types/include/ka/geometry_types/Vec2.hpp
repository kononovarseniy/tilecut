#pragma once

#include <source_location>

#include <ka/common/cast.hpp>
#include <ka/common/fixed.hpp>
#include <ka/common/hash.hpp>

namespace ka
{

template <typename T>
struct Vec2 final
{
    using CoordinateType = T;

    T x;
    T y;

    void hash(Hasher & hasher) const noexcept
    {
        hasher.update(x);
        hasher.update(y);
    }

    [[nodiscard]] constexpr auto operator<=>(const Vec2 &) const noexcept = default;

    template <typename Target>
    constexpr Vec2<Target> exact_cast(
        const std::source_location & location = std::source_location::current()) const noexcept
    {
        return {
            .x = ka::exact_cast<Target>(x, location),
            .y = ka::exact_cast<Target>(y, location),
        };
    }
};

template <typename T>
concept IsVec2 = std::same_as<T, Vec2<typename T::CoordinateType>>;

using Vec2u8 = Vec2<u8>;
using Vec2u16 = Vec2<u16>;
using Vec2u32 = Vec2<u32>;
using Vec2u64 = Vec2<u64>;
using Vec2s8 = Vec2<s8>;
using Vec2s16 = Vec2<s16>;
using Vec2s32 = Vec2<s32>;
using Vec2s64 = Vec2<s64>;
using Vec2f32 = Vec2<f32>;
using Vec2f64 = Vec2<f64>;

} // namespace ka
