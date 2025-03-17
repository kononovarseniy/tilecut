#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <ka/common/cast.hpp>
#include <ka/exact/GridRounding.hpp>
#include <ka/geometry_types/Vec2.hpp>
#include <ka/tilecut/LineSnapper.hpp>
#include <ka/tilecut/LineSnapperCoordinateHandler.hpp>
#include <ka/tilecut/lerp_along_segment.hpp>

#include "debug_output.hpp"
#include "mock_grid_parameters.hpp"

namespace ka
{

using ::testing::ElementsAreArray;

class TestCoordinateHandler final
{
public:
    struct InputVertex final
    {
        Vec2f64 xy;
        f64 z;
    };

    struct OutputVertex final
    {
        Vec2s64 xy;
        s32 z;

        [[nodiscard]] constexpr bool operator==(const OutputVertex &) const noexcept = default;
    };

public:
    [[nodiscard]] Vec2f64 project(const InputVertex & vertex) const noexcept
    {
        return vertex.xy;
    }

    [[nodiscard]] OutputVertex transform(const InputVertex & vertex, const Vec2s64 & position) const noexcept
    {
        return {
            .xy = position,
            .z = exact_cast<s32>(std::round(vertex.z)),
        };
    }

    [[nodiscard]] OutputVertex interpolate(
        const InputVertex & start_in,
        const OutputVertex & start_out,
        const InputVertex & stop_in,
        const OutputVertex & stop_out,
        const Vec2s64 & position) const noexcept
    {
        const auto interpolated_z = lerp_along_segment(
            start_out.xy.exact_cast<f64>(),
            start_in.z,
            stop_out.xy.exact_cast<f64>(),
            stop_in.z,
            position.exact_cast<f64>());
        return {
            .xy = position,
            .z = exact_cast<s32>(std::round(interpolated_z)),
        };
    }
};

std::ostream & operator<<(std::ostream & stream, const TestCoordinateHandler::OutputVertex & vertex)
{
    stream << fmt::format("(({}, {}), {})", vertex.xy.x, vertex.xy.y, vertex.z);
    return stream;
}

class Test2DCoordinateHandler final
{
public:
    using InputVertex = Vec2f64;

    using OutputVertex = Vec2s64;

public:
    [[nodiscard]] Vec2f64 project(const InputVertex & vertex) const noexcept
    {
        return vertex;
    }

    [[nodiscard]] OutputVertex transform(const InputVertex &, const Vec2s64 & position) const noexcept
    {
        return position;
    }

    [[nodiscard]] OutputVertex interpolate(
        const InputVertex &,
        const OutputVertex &,
        const InputVertex &,
        const OutputVertex &,
        const Vec2s64 & position) const noexcept
    {
        return position;
    }
};

static_assert(LineSnapperCoordinateHandler<TestCoordinateHandler>);

TEST(LineSnapperTest, interface_test)
{
    const auto grid = make_grid<GridRounding::NearestNode>(1.0, {}, 10);

    std::vector<TestCoordinateHandler::InputVertex> vertices {
        { { -20.3, 5.0 }, -100.23 },
        { { 20.3, 5.0 }, 100.23 },
    };
    // clang-format off
    std::vector<TestCoordinateHandler::OutputVertex> expected {
        { { -20, 5 }, -100 },
        { { -10, 5 }, -50 },
        { { 0, 5 }, 0 },
        { { 10, 5 }, 50 },
        { { 20, 5 }, 100 },
    };
    // clang-format on
    std::vector<TestCoordinateHandler::OutputVertex> result;

    TestCoordinateHandler handler;

    LineSnapper snapper;
    snapper.snap_line(grid, handler, vertices, std::back_inserter(result));

    EXPECT_EQ(result, expected);
}

TEST(LineSnapperTest, interface_2d_test)
{
    const auto grid = make_grid<GridRounding::NearestNode>(1.0, {}, 10);

    std::vector<Vec2f64> vertices {
        { -20.3, 5.0 },
        { 20.3, 5.0 },
    };
    // clang-format off
    std::vector<Vec2s64> expected {
        { -21, 5 },
        { -20, 5 },
        { -10, 5 },
        { 0, 5 },
        { 10, 5 },
        { 20, 5 },
    };
    // clang-format on
    std::vector<Vec2s64> result;

    Test2DCoordinateHandler handler;

    LineSnapper snapper;
    snapper.snap_line(grid, handler, vertices, std::back_inserter(result));
}

TEST(LineSnapperTest, empty_2d_test)
{
    const auto grid = make_grid<GridRounding::NearestNode>(1.0, {}, 10);

    std::vector<Vec2f64> vertices;
    std::vector<Vec2s64> expected;
    std::vector<Vec2s64> result;

    Test2DCoordinateHandler handler;

    LineSnapper snapper;
    snapper.snap_line(grid, handler, vertices, std::back_inserter(result));

    EXPECT_EQ(result, expected);
}

} // namespace ka
