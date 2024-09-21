#pragma once

#include <common/fixed.hpp>

namespace ka::tilecut
{

struct uvec16 final
{
    r7::u16 x;
    r7::u16 y;

    [[nodiscard]] constexpr auto operator<=>(const uvec16 &) const noexcept = default;
};

} // namespace ka::tilecut
