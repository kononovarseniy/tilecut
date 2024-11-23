#pragma once

#include <common/fixed.hpp>
#include <common/hash.hpp>

namespace ka
{

template <typename T>
struct Vec2 final
{
    T x;
    T y;

    void hash(Hasher & hasher) const noexcept
    {
        hasher.update(x);
        hasher.update(y);
    }

    [[nodiscard]] constexpr auto operator<=>(const Vec2 &) const noexcept = default;
};

using Vec2f64 = Vec2<f64>;
using Vec2s64 = Vec2<s64>;
using Vec2s32 = Vec2<s32>;
using Vec2u16 = Vec2<u16>;

} // namespace ka
