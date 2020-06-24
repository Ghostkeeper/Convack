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
	ASSERT_EQ(identity.apply(Point2(13, 37)), Point2(13, 37)) << "The identity transformation should not modify the point.";
}

}