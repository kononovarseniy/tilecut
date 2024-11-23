/// @file
/// Bibliography:
/// *   [Shewchuk] J. R. Shewchuk. 1996
///     Adaptive Precision Floating-Point Arithmetic and Fast Robust Geometric Predicates
/// *   [Dekker] T. J. Dekker. 1971
///     A Floating-Point Technique for Extending the Available Precision
/// *   [Knuth] D. E. Knuth. 1981
///     The Art of Computer Programming: Seminumerical Algorithms, second edition, volume 2.
#pragma once

#include <algorithm>
#include <concepts>
#include <span>

#include <ka/common/assert.hpp>
#include <ka/common/fixed.hpp>

namespace ka
{

/// @brief Basic expansion with two components.
/// @invariant |approx| > |err|.
/// @tparam T type of expansion components.
template <std::floating_point T>
class TwoExpansion final
{
public:
    constexpr explicit TwoExpansion(const T approx) noexcept
        : expansion_ { T {}, approx }
    {
    }

    constexpr TwoExpansion(const std::array<T, 2> & expansion) noexcept
        : expansion_ { expansion }
    {
    }

    constexpr TwoExpansion & operator=(const std::array<T, 2> & expansion) noexcept
    {
        expansion_ = expansion;
        return *this;
    }

public:
    /// @brief Roundoff error.
    constexpr T err() const
    {
        return expansion_[0];
    }

    /// @brief Approximation.
    constexpr T approx() const
    {
        return expansion_[1];
    }

private:
    std::array<T, 2> expansion_;
};

/// @brief Exact sum of two pre-ordered numbers.
/// [Shewchuk] [Dekker]
/// @pre |a| >= |b| or any of them is zero.
/// @post result is nonoverlapping.
/// @post result is nonadjacent if round-to-even tiebreaking is used.
/// @tparam T type of expansion components.
template <std::floating_point T>
[[nodiscard]] constexpr std::array<T, 2> fast_two_sum(const T a, const T b) noexcept
{
    AR_PRE(std::abs(a) >= std::abs(b) || a == 0.0 || b == 0.0);
    const auto approx = a + b;
    const auto b_virtual = approx - a;
    const auto err = b - b_virtual;
    return { err, approx };
}

/// @brief Exact difference of two pre-ordered numbers.
/// [Shewchuk] [Dekker]
/// @pre |a| >= |b| or any of them is zero.
/// @post result is nonoverlapping.
/// @post result is nonadjacent if round-to-even tiebreaking is used.
/// @tparam T type of expansion components.
template <std::floating_point T>
[[nodiscard]] constexpr std::array<T, 2> fast_two_diff(const T a, const T b) noexcept
{
    AR_PRE(std::abs(a) >= std::abs(b) || a == 0.0 || b == 0.0);
    const auto approx = a - b;
    const auto b_virtual = a - approx;
    const auto err = b_virtual - b;
    return { err, approx };
}

/// @brief Exact sum of two numbers.
/// [Shewchuk] [Knuth]
/// @post result is nonoverlapping.
/// @post result is nonadjacent if round-to-even tiebreaking is used.
/// @tparam T type of expansion components.
template <std::floating_point T>
[[nodiscard]] constexpr std::array<T, 2> two_sum(const T a, const T b) noexcept
{
    const auto approx = a + b;
    const auto b_virtual = approx - a;
    const auto a_virtual = approx - b_virtual;
    const auto b_roundoff = b - b_virtual;
    const auto a_roundoff = a - a_virtual;
    const auto err = a_roundoff + b_roundoff;
    return { err, approx };
}

/// @brief Exact difference of two numbers.
/// [Shewchuk] [Knuth]
/// @post result is nonoverlapping.
/// @post result is nonadjacent if round-to-even tiebreaking is used.
/// @tparam T type of expansion components.
template <std::floating_point T>
[[nodiscard]] constexpr std::array<T, 2> two_diff(const T a, const T b) noexcept
{
    const auto approx = a - b;
    const auto b_virtual = a - approx;
    const auto a_virtual = approx + b_virtual;
    const auto b_roundoff = b_virtual - b;
    const auto a_roundoff = a - a_virtual;
    const auto err = a_roundoff + b_roundoff;
    return { err, approx };
}

/// @brief Sum of expansion and number.
/// Preserves nonoverlapping property.
/// Preserves nonadjacent property if round-to-even tiebreaking is used.
/// [Shewchuk]
/// @pre If the expansion and result spans are overlapping in memory,
/// then the address of result span is not greater than the address of expansion span.
/// @tparam T type of expansion components.
/// @tparam Size number of expansion components.
template <std::floating_point T, size_t Size>
constexpr void grow_expansion(
    const std::span<const T, Size> expansion,
    const T number,
    const std::span<T, Size + 1> result) noexcept
{
    AR_PRE(
        &expansion.back() < &result.front() || &result.back() < &expansion.front() ||
        &result.front() <= &expansion.front());
    TwoExpansion sum { number };
    for (size_t i = 0; i < expansion.size(); ++i)
    {
        sum = two_sum(sum.approx(), expansion[i]);
        result[i] = sum.err();
    }
    result.back() = sum.approx();
}

/// @brief Computes lhs + sign * rhs.
template <int sign, std::floating_point T, size_t LSize, size_t RSize>
constexpr void expansion_sum_impl(
    const std::span<const T, LSize> lhs,
    const std::span<const T, RSize> rhs,
    const std::span<T, LSize + RSize> result) noexcept
{
    std::ranges::copy(lhs, result.begin());
    for (size_t i = 0; i < rhs.size(); ++i)
    {
        grow_expansion(
            std::span<const T, LSize>(result.subspan(i, LSize)),
            sign * rhs[i],
            std::span<T, LSize + 1>(result.subspan(i, LSize + 1)));
    }
}

/// @brief Sum of two expansions.
/// Preserves nonoverlapping property.
/// Does not preserve strongly nonoverlapping property.
/// Preserves nonajacency if round-to-even tiebreaking is used.
/// 6 * LSize * Rsize flops.
/// @tparam T type of expansion components.
/// @tparam LSize number of the lhs components.
/// @tparam RSize number of the rhs components.
template <std::floating_point T, size_t LSize, size_t RSize>
constexpr void expansion_sum(
    const std::span<const T, LSize> lhs,
    const std::span<const T, RSize> rhs,
    const std::span<T, LSize + RSize> result) noexcept
{
    expansion_sum_impl<+1>(lhs, rhs, result);
}

/// @brief Difference of two expansions.
/// Preserves nonoverlapping property.
/// Does not preserve strongly nonoverlapping property.
/// Preserves nonajacency if round-to-even tiebreaking is used.
/// 6 * LSize * Rsize flops.
/// @tparam T type of expansion components.
/// @tparam LSize number of the lhs components.
/// @tparam RSize number of the rhs components.
template <std::floating_point T, size_t LSize, size_t RSize>
constexpr void expansion_diff(
    const std::span<const T, LSize> lhs,
    const std::span<const T, RSize> rhs,
    const std::span<T, LSize + RSize> result) noexcept
{
    expansion_sum_impl<-1>(lhs, rhs, result);
}

/// @brief Computes lhs + sign * rhs.
template <int sign, std::floating_point T, size_t LSize, size_t RSize>
constexpr void fast_expansion_sum_impl(
    const std::span<const T, LSize> lhs,
    const std::span<const T, RSize> rhs,
    const std::span<T, LSize + RSize> result) noexcept
{
    // Checks that |a| < |b|.
    const auto increasing = [](const auto a, const auto b) noexcept
    {
        return (a < b) == (-a < b);
    };
    auto lhs_it = lhs.begin();
    auto rhs_it = rhs.begin();
    const auto next_merged = [&]()
    {
        if (increasing(*lhs_it, sign * *rhs_it))
        {
            return *lhs_it++;
        }
        else
        {
            return sign * *rhs_it++;
        }
    };

    const auto merged_0 = next_merged();
    const auto merged_1 = next_merged();
    TwoExpansion sum_err = fast_two_sum(merged_1, merged_0);
    auto result_it = result.begin();
    *result_it++ = sum_err.err();

    while (lhs_it != lhs.end() && rhs_it != rhs.end())
    {
        sum_err = two_sum(sum_err.approx(), next_merged());
        *result_it++ = sum_err.err();
    }
    while (lhs_it != lhs.end())
    {
        sum_err = two_sum(sum_err.approx(), *lhs_it);
        ++lhs_it;
        *result_it++ = sum_err.err();
    }
    while (rhs_it != rhs.end())
    {
        sum_err = two_sum(sum_err.approx(), sign * *rhs_it);
        ++rhs_it;
        *result_it++ = sum_err.err();
    }
    *result_it = sum_err.approx();
}

/// @brief Sum of two strongly nonoverlapping expansions.
/// Preserves nonoverlapping property.
/// 6 * LSize + 6 * Rsize - 9 flops.
/// m + n - 1 comparisons by absolute value.
/// @pre Round-to-even tiebreaking is used.
/// @tparam T type of expansion components.
/// @tparam LSize number of the lhs components.
/// @tparam RSize number of the rhs components.
template <std::floating_point T, size_t LSize, size_t RSize>
constexpr void fast_expansion_sum(
    const std::span<const T, LSize> lhs,
    const std::span<const T, RSize> rhs,
    const std::span<T, LSize + RSize> result) noexcept
{
    fast_expansion_sum_impl<+1>(lhs, rhs, result);
}

/// @brief Difference of two strongly nonoverlapping expansions.
/// Preserves nonoverlapping property.
/// 6 * LSize + 6 * Rsize - 9 flops.
/// m + n - 1 comparisons by absolute value.
/// @pre Round-to-even tiebreaking is used.
/// @tparam T type of expansion components.
/// @tparam LSize number of the lhs components.
/// @tparam RSize number of the rhs components.
template <std::floating_point T, size_t LSize, size_t RSize>
constexpr void fast_expansion_difference(
    const std::span<const T, LSize> lhs,
    const std::span<const T, RSize> rhs,
    const std::span<T, LSize + RSize> result) noexcept
{
    fast_expansion_sum_impl<-1>(lhs, rhs, result);
}

/// @brief The result of a split operation.
/// @invariant |hi| > |lo|.
template <std::floating_point T>
struct SplitResult final
{
    T hi;
    T lo;
};

/// @brief Splits the value to the sum of two nonoverlapping numbers hi and lo
/// of size P-S and S-1 bits conrespondingly, where P is std::numeric_limits<T>::digits().
/// [Shewchuk] [Dekker]
template <u8 S, std::floating_point T>
[[nodiscard]] SplitResult<T> split(const T value)
{
    const auto c = ((1 << S) + 1) * value;
    const auto big = c - value;
    const auto hi = c - big;
    const auto lo = value - hi;
    return { hi, lo };
}

/// @brief Product of two numbers.
/// Preserves nonoverlapping property.
/// Preserves nonadjacent property if round-to-even tiebreaking is used.
/// [Shewchuk]
/// @tparam T type of expansion components.
template <std::floating_point T>
[[nodiscard]] constexpr std::array<T, 2> two_product(const T lhs, const T rhs)
{
    constexpr auto split_point = (std::numeric_limits<T>::digits + 1) / 2;
    const auto approx = lhs * rhs;
    const auto a = split<split_point>(lhs);
    const auto b = split<split_point>(rhs);
    auto err = approx - a.hi * b.hi;
    err -= a.lo * b.hi;
    err -= a.hi * b.lo;
    err = a.lo * b.lo - err;
    return { err, approx };
}

/// @brief Product of expansion and number.
/// Preserves nonoverlapping property.
/// Preserves nonadjacent property if round-to-even tiebreaking is used.
/// [Shewchuk]
/// @tparam T type of expansion components.
/// @tparam Size number of expansion components.
template <std::floating_point T, size_t Size>
constexpr void scale_expansion(
    const std::span<const T, Size> expansion,
    const T number,
    const std::span<T, 2 * Size> result)
{
    auto result_it = result.begin();

    TwoExpansion prod_err = two_product(expansion.front(), number);
    *result_it++ = prod_err.err();

    for (size_t i = 1; i < expansion.size(); ++i)
    {
        const TwoExpansion t = two_product(expansion[i], number);
        prod_err = two_sum(prod_err.approx(), t.err());
        *result_it++ = prod_err.err();
        prod_err = fast_two_sum(t.approx(), prod_err.approx());
        *result_it++ = prod_err.err();
    }
    *result_it++ = prod_err.approx();
}

/// @brief Finds leading non-zero component of the expansion.
/// @tparam T type of expansion components.
/// @tparam Size number of expansion components.
template <typename T, size_t Size>
[[nodiscard]] constexpr T expansion_approx(const std::span<const T, Size> expansion) noexcept
{
    for (ptrdiff_t i = expansion.size() - 1; i >= 0; i--)
    {
        if (expansion[i] != 0)
        {
            return expansion[i];
        }
    }
    return {};
}

} // namespace ka
