#pragma once

#include <common/fixed.hpp>
#include <common/hash.hpp>

namespace r7
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

} // namespace r7
