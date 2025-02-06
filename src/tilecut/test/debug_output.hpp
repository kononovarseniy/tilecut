#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <iostream>

#include <fmt/format.h>

#include <ka/geometry_types/Segment2.hpp>
#include <ka/geometry_types/Vec2.hpp>

namespace ka
{

template <typename T>
std::ostream & operator<<(std::ostream & stream, const Vec2<T> & point)
{
    stream << fmt::format("({}, {})", point.x, point.y);
    return stream;
}

template <typename T>
std::ostream & operator<<(std::ostream & stream, const Segment2<T> & segment)
{
    stream << fmt::format("({}, {})->({}, {})", segment.a.x, segment.a.y, segment.b.x, segment.b.y);
    return stream;
}

} // namespace ka
