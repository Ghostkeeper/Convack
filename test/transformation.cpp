/*
 * Library to pack convex polygons into arbitrary shapes.
 * Any copyright is dedicated to the public domain. See LICENSE.md for more details.
 */

#include <cmath> //To construct turns in radians.
#include <gtest/gtest.h> //To run the test.

#include "point2.hpp" //To create points to transform.
#include "transformation.hpp" //The unit under test.

namespace convack {

/*!
 * Helper function that asserts that two points are equal, allowing for some
 * rounding errors.
 */
void expect_points_eq(const Point2& a, const Point2 b, const std::string& message) {
	EXPECT_NEAR(a.x, b.x, 0.0000001) << message;
	EXPECT_NEAR(a.y, b.y, 0.0000001) << message;
}

/*!
 * Test the identity transformation created by the constructor.
 */
TEST(Transformation, Identity) {
	Transformation identity;
	ASSERT_EQ(identity.apply(Point2(0, 0)), Point2(0, 0)) << "The identity transformation should not modify the point.";
	ASSERT_EQ(identity.apply(Point2(13, 37)), Point2(13, 37)) << "The identity transformation should not modify the point.";
}

/*!
 * Test translating a point.
 */
TEST(Transformation, Translation) {
	Transformation translation = Transformation().translate(5, -2);
	ASSERT_EQ(translation.apply(Point2(0, 0)), Point2(5, -2)) << "The translation should move the point +5 in X, and -2 in Y.";
	ASSERT_EQ(translation.apply(Point2(8, 39)), Point2(13, 37)) << "The translation should move the point +5 in X, and -2 in Y.";
}

/*!
 * Test rotating a point around 0,0.
 */
TEST(Transformation, Rotation) {
	const double pi = std::acos(-1);
	Transformation rotation = Transformation().rotate(pi / 2); //Rotate a quarter turn anticlockwise.
	expect_points_eq(rotation.apply(Point2(0, 0)), Point2(0, 0), "Rotating a point at 0,0 around 0,0 has no effect.");
	expect_points_eq(rotation.apply(Point2(42, 0)), Point2(0, 42), "This point should get rotated a quarter turn anticlockwise around 0,0.");

	rotation.rotate(pi); //Rotate another half turn, so now in total 3/4 turn.
	expect_points_eq(rotation.apply(Point2(42, 0)), Point2(0, -42), "This point was rotated 3/4 turn counterclockwise (1/4 turn clockwise), and so now points down instead of to the right.");
}

}