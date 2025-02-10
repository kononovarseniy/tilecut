#pragma once

#include <algorithm>
#include <iterator>
#include <utility>

#include <ka/common/fixed.hpp>
#include <ka/geometry_types/Segment2.hpp>
#include <ka/geometry_types/Vec2.hpp>

namespace ka
{

/// @brief Helper class providing methods for mapping geometry to tiles it passes through.
class TileGrid final
{
public:
    explicit TileGrid(const u16 tile_size) noexcept
        : tile_size_ { tile_size }
    {
        AR_PRE(tile_size > 0);
    }

public:
    [[nodiscard]] u16 tile_size() const noexcept
    {
        return tile_size_;
    }

public:
    /// @brief Returns the coordinates of the tile containing the given cell.
    [[nodiscard]] Vec2s64 tile_of(const Vec2s64 & cell) const noexcept
    {
        return {
            .x = div_round_down(cell.x, tile_size_),
            .y = div_round_down(cell.y, tile_size_),
        };
    }

    /// @brief Returns the coordinates of the tile containing the given segment.
    /// @note Segments that lie entirely on the boundary of a tile are considered to belong to the tile that is in the
    /// left half-plane relative to that segment.
    /// Thanks to this property, a tile will never contain a 2D part of a polygon.
    /// In addition, this makes it a little easier to find the boundaries of a tile lying inside a polygon.
    [[nodiscard]] Vec2s64 tile_of(const Segment2s64 & segment) const noexcept
    {
        AR_PRE(segment.a != segment.b);
        AR_PRE(is_inside_single_tile(segment));

        const auto start_tile = tile_of(segment.a);
        const auto stop_tile = tile_of(segment.b);
        const auto tile_x = std::min(start_tile.x, stop_tile.x);
        const auto tile_y = std::min(start_tile.y, stop_tile.y);

        if (segment.a.x == segment.b.x && segment.a.x == tile_x * tile_size_)
        {
            return {
                segment.a.y < segment.b.y ? tile_x - 1 : tile_x,
                tile_y,
            };
        }
        if (segment.a.y == segment.b.y && segment.a.y == tile_y * tile_size_)
        {
            return {
                tile_x,
                segment.a.x < segment.b.x ? tile_y : tile_y - 1,
            };
        }
        return { tile_x, tile_y };
    }

    /// @brief Converts cell coordinates to the local coordinates of the given tile.
    [[nodiscard]] Vec2u16 local_coordinates(const Vec2s64 & tile, const Vec2s64 & cell) const noexcept
    {
        const Vec2s64 local {
            .x = cell.x - tile.x * tile_size_,
            .y = cell.y - tile.y * tile_size_,
        };
        AR_PRE(local.x >= 0);
        AR_PRE(local.x <= tile_size_);
        AR_PRE(local.y >= 0);
        AR_PRE(local.y <= tile_size_);
        return local.exact_cast<u16>();
    }

    /// @brief Converts segment coordinates to the local coordinates of the given tile.
    [[nodiscard]] Segment2u16 local_coordinates(const Vec2s64 & tile, const Segment2s64 & segment) const noexcept
    {
        return {
            .a = local_coordinates(tile, segment.a),
            .b = local_coordinates(tile, segment.b),
        };
    }

    /// @brief Checks that a segment is entirely contained within a single tile.
    [[nodiscard]] bool is_inside_single_tile(const Segment2s64 & segment) const noexcept
    {
        return is_inside_single_tile(segment.a.x, segment.b.x) && is_inside_single_tile(segment.a.y, segment.b.y);
    }

    /// @brief Checks that the cell does not belong to the tile closure.
    [[nodiscard]] bool strictly_outside(const Vec2s64 & tile, const Vec2s64 & cell) const noexcept
    {
        const auto left = tile.x * tile_size_;
        const auto right = left + tile_size_;
        const auto bottom = tile.y * tile_size_;
        const auto top = bottom + tile_size_;
        return cell.x < left || cell.x > right || cell.y < bottom || cell.y > top;
    }

    struct BoundariesRanges final
    {
        s64 min_x;
        s64 max_x;
        s64 min_y;
        s64 max_y;
    };

    /// @brief Returns the coordinate ranges of the tile boundaries intersected by the given segment.
    [[nodiscard]] BoundariesRanges intersected_boundaries_ranges(const Segment2s64 & segment) const noexcept
    {
        const auto [min_x, max_x] = intersected_boundaries_range(segment.a.x, segment.b.x);
        const auto [min_y, max_y] = intersected_boundaries_range(segment.a.y, segment.b.y);
        return { min_x, max_x, min_y, max_y };
    }

    /// @brief Returns the coordinates of the first and last tile boundaries between the given cell coordinates
    /// (inclusive, unordered).
    /// @return A pair of minimum and maximum boundary coordinates.
    /// If the first value of a pair is greater than the second, then there is no boundary intersections.
    [[nodiscard]] std::pair<s64, s64> intersected_boundaries_range(s64 begin_cell, s64 end_cell) const noexcept
    {
        if (begin_cell > end_cell)
        {
            std::swap(begin_cell, end_cell);
        }
        return {
            div_round_up(begin_cell, tile_size_) * tile_size_,
            div_round_down(end_cell, tile_size_) * tile_size_,
        };
    }

    /// @brief Writes counterclockwise tile boundary segments in local coordinates to the specified range.
    template <std::output_iterator<Segment2u16> Out>
    Out tile_local_boundaries(Out out) const noexcept
    {
        const std::array<Vec2u16, 4> corners { {
            { 0, 0 },
            { tile_size_, 0 },
            { tile_size_, tile_size_ },
            { 0, tile_size_ },
        } };
        for (size_t i = 0; i < corners.size(); ++i)
        {
            *out++ = { corners[i], corners[(i + 1) % corners.size()] };
        }
        return out;
    }

private:
    [[nodiscard]] bool is_inside_single_tile(s64 a, s64 b) const noexcept
    {
        if (a > b)
        {
            std::swap(a, b);
        }
        const auto min_tile = div_round_down(a, tile_size_);
        const auto max_tile = div_round_down(b, tile_size_);
        return min_tile == max_tile || b == (min_tile + 1) * tile_size_;
    }

    [[nodiscard]] static constexpr s64 div_round_up(const s64 a, const u16 b) noexcept
    {
        AR_PRE(b > 0);
        if (a >= 0)
        {
            return (a + b - 1) / b;
        }
        return -(-a / b);
    }

    [[nodiscard]] static constexpr s64 div_round_down(const s64 a, const u16 b) noexcept
    {
        AR_PRE(b > 0);
        if (a >= 0)
        {
            return a / b;
        }
        return -((-a + b - 1) / b);
    }

private:
    u16 tile_size_;
};

} // namespace ka
