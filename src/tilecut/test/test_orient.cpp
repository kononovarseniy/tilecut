#include <gtest/gtest.h>

#include <cmath>

#include <ka/tilecut/orient.hpp>

namespace ka
{

inline namespace
{

KA_TILECUT_ORIENTATION_CLASSIFIER(TestClassifier, gt, ge, eq, le, lt);

} // namespace

TEST(OrientTest, classsifier_equality_test)
{
    EXPECT_EQ(TestClassifier<f64> { -1.0 }, TestClassifier<f64> { -1.0 });
    EXPECT_EQ(TestClassifier<f64> { 0.0 }, TestClassifier<f64> { 0.0 });
    EXPECT_EQ(TestClassifier<f64> { 1.0 }, TestClassifier<f64> { 1.0 });

    EXPECT_EQ(TestClassifier<f64> { -1.0 }, TestClassifier<f64> { -234.0 });
    EXPECT_EQ(TestClassifier<f64> { 1.0 }, TestClassifier<f64> { 234.0 });

    EXPECT_NE(TestClassifier<f64> { -1.0 }, TestClassifier<f64> { 1.0 });
    EXPECT_NE(TestClassifier<f64> { -1.0 }, TestClassifier<f64> { 0.0 });
    EXPECT_NE(TestClassifier<f64> { 1.0 }, TestClassifier<f64> { 0.0 });
}

// We are not testing different coordinate types, as we do not test the underlying predicate.
// We are only testing the classifier methods and the usage of the predicate.
TEST(OrientTest, point_location_test)
{
    {
        const auto result = point_location<s16>({ 1, 0 }, { 0, 1 }, { -1, 0 });
        const auto result_segment = point_location<s16>({ { 1, 0 }, { 0, 1 } }, { -1, 0 });
        EXPECT_EQ(result, result_segment);
        EXPECT_TRUE(result.left());
        EXPECT_TRUE(result.left_or_line());
        EXPECT_FALSE(result.line());
        EXPECT_FALSE(result.right_or_line());
        EXPECT_FALSE(result.right());
    }
    {
        const auto result = point_location<s16>({ 1, 0 }, { 0, 0 }, { -1, 0 });
        const auto result_segment = point_location<s16>({ { 1, 0 }, { 0, 0 } }, { -1, 0 });
        EXPECT_EQ(result, result_segment);
        EXPECT_FALSE(result.left());
        EXPECT_TRUE(result.left_or_line());
        EXPECT_TRUE(result.line());
        EXPECT_TRUE(result.right_or_line());
        EXPECT_FALSE(result.right());
    }
    {
        const auto result = point_location<s16>({ -1, 0 }, { 0, 1 }, { 1, 0 });
        const auto result_segment = point_location<s16>({ { -1, 0 }, { 0, 1 } }, { 1, 0 });
        EXPECT_EQ(result, result_segment);
        EXPECT_FALSE(result.left());
        EXPECT_FALSE(result.left_or_line());
        EXPECT_FALSE(result.line());
        EXPECT_TRUE(result.right_or_line());
        EXPECT_TRUE(result.right());
    }
}

TEST(OrientationTest, test_point_order)
{
    {
        const auto result = point_order<s16>({ 1, 0 }, { 0, 1 }, { -1, 0 });
        EXPECT_TRUE(result.is_ccw());
        EXPECT_TRUE(result.is_ccw_or_collinear());
        EXPECT_FALSE(result.is_collinear());
        EXPECT_FALSE(result.is_cw_or_collinear());
        EXPECT_FALSE(result.is_cw());
    }
    {
        const auto result = point_order<s16>({ 1, 0 }, { 0, 0 }, { -1, 0 });
        EXPECT_FALSE(result.is_ccw());
        EXPECT_TRUE(result.is_ccw_or_collinear());
        EXPECT_TRUE(result.is_collinear());
        EXPECT_TRUE(result.is_cw_or_collinear());
        EXPECT_FALSE(result.is_cw());
    }
    {
        const auto result = point_order<s16>({ -1, 0 }, { 0, 1 }, { 1, 0 });
        EXPECT_FALSE(result.is_ccw());
        EXPECT_FALSE(result.is_ccw_or_collinear());
        EXPECT_FALSE(result.is_collinear());
        EXPECT_TRUE(result.is_cw_or_collinear());
        EXPECT_TRUE(result.is_cw());
    }
}

TEST(OrientationTest, test_vertex_type)
{
    {
        const auto result = vertex_type<s16>({ 1, 0 }, { 0, 1 }, { -1, 0 });
        EXPECT_TRUE(result.is_convex());
        EXPECT_TRUE(result.is_convex_or_straight());
        EXPECT_FALSE(result.is_straight());
        EXPECT_FALSE(result.is_reflex_or_straight());
        EXPECT_FALSE(result.is_reflex());
    }
    {
        const auto result = vertex_type<s16>({ 1, 0 }, { 0, 0 }, { -1, 0 });
        EXPECT_FALSE(result.is_convex());
        EXPECT_TRUE(result.is_convex_or_straight());
        EXPECT_TRUE(result.is_straight());
        EXPECT_TRUE(result.is_reflex_or_straight());
        EXPECT_FALSE(result.is_reflex());
    }
    {
        const auto result = vertex_type<s16>({ -1, 0 }, { 0, 1 }, { 1, 0 });
        EXPECT_FALSE(result.is_convex());
        EXPECT_FALSE(result.is_convex_or_straight());
        EXPECT_FALSE(result.is_straight());
        EXPECT_TRUE(result.is_reflex_or_straight());
        EXPECT_TRUE(result.is_reflex());
    }
}

} // namespace ka
