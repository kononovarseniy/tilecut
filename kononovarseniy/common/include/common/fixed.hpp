#pragma once

#include <cstdint>
#include <limits>

namespace ka
{

using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;

using s8 = std::int8_t;
using s16 = std::int16_t;
using s32 = std::int32_t;
using s64 = std::int64_t;

using f32 = float;
using f64 = double;

static_assert(std::numeric_limits<f32>::is_iec559);
static_assert(std::numeric_limits<f64>::is_iec559);

static_assert(std::numeric_limits<f32>::digits == 24);
static_assert(std::numeric_limits<f64>::digits == 53);

} // namespace ka
