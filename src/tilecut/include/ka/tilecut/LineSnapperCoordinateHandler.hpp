#pragma once

#include <concepts>
#include <utility>

#include <ka/geometry_types/Vec2.hpp>

namespace ka
{

template <typename H>
concept LineSnapperCoordinateHandler = requires(
    const H & handler,
    const typename H::InputVertex & vertex_in,
    const typename H::OutputVertex & vertex_out,
    const Vec2s64 & position) {
    requires std::movable<typename H::InputVertex>;
    requires std::movable<typename H::OutputVertex>;

    { handler.project(vertex_in) } -> std::convertible_to<Vec2f64>;
    { handler.transform(vertex_in, position) } -> std::convertible_to<typename H::OutputVertex>;
    {
        handler.interpolate(vertex_in, vertex_out, vertex_in, vertex_out, position)
    } -> std::convertible_to<typename H::OutputVertex>;
};

} // namespace ka
