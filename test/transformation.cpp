/*
 * Library to pack convex polygons into arbitrary shapes.
 * Any copyright is dedicated to the public domain. See LICENSE.md for more details.
 */

#include <gtest/gtest.h> //To run the test.

#include "point2.hpp" //To create points to transform.
#include "transformation.hpp" //The unit under test.

namespace convack {

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

}