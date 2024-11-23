#pragma once

#include <ka/common/fixed.hpp>

namespace ka
{

[[nodiscard]] bool check_column_border_intersecion(
    const f64 a_x,
    const f64 a_y,
    const f64 b_x,
    const f64 b_y,
    const f64 size,
    const s64 c_x,
    const s64 c_y);

} // namespace ka
