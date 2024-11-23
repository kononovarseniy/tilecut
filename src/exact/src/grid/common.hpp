#pragma once

#include <array>
#include <cstddef>
#include <span>

#include <ka/common/fixed.hpp>

namespace ka
{

inline namespace
{

template <typename T, size_t E>
[[nodiscard]] constexpr auto span(std::array<T, E> & array) noexcept
{
    return std::span<T, E> { array };
}

template <typename T, size_t E>
[[nodiscard]] constexpr auto const_span(const std::array<T, E> & array) noexcept
{
    return std::span<const T, E> { array };
}

[[nodiscard]] [[maybe_unused]] s64 half_cell_to_nearest_full_cell(const s64 value) noexcept
{
    if (value >= -1)
    {
        return (value + 1) / 2;
    }
    return (value - 1) / 2;
}

} // namespace

} // namespace ka
