#pragma once

#include <concepts>
#include <utility>

#include <ka/geometry_types/Vec2.hpp>

namespace ka
{

template <typename Handler>
concept LineSnapperCoordinateHandler = requires() {
    typename Handler::InputVertex;
    typename Handler::OutputVertex;

    requires requires(
        const Handler & handler,
        const typename Handler::InputVertex & vertex_in,
        const typename Handler::OutputVertex & vertex_out,
        const Vec2s64 & position) {
        { handler.project(vertex_in) } -> std::same_as<Vec2f64>;
        { handler.transform(vertex_in, position) } -> std::same_as<typename Handler::OutputVertex>;
        {
            handler.interpolate(vertex_in, vertex_out, vertex_in, vertex_out, position)
        } -> std::same_as<typename Handler::OutputVertex>;
    };
};

} // namespace ka
