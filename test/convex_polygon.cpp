/*
 * Library to pack convex polygons into arbitrary shapes.
 * Any copyright is dedicated to the public domain. See LICENSE.md for more details.
 */

#include <gtest/gtest.h> //To run the test.
#include <vector> //To store vertices of test polygons.
#include <convex_polygon.hpp> //The unit under test.

namespace convack {

/*!
 * Test constructing a convex polygon from a predefined set of vertices.
 */
TEST(ConvexPolygon, ConstructEmpty) {
	std::vector<Point2> vertices = {};
	const ConvexPolygon poly(vertices);
	EXPECT_EQ(poly.get_vertices().size(), 0) << "There were no vertices in the input.";
}

}