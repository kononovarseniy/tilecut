#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <iostream>

#include <fmt/format.h>

#include <ka/geometry_types/Vec2.hpp>

namespace ka
{

template <typename T>
std::ostream & operator<<(std::ostream & stream, const Vec2<T> & point)
{
    stream << fmt::format("({}, {})", point.x, point.y);
    return stream;
}

} // namespace ka
