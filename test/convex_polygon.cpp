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

/*!
 * Tests constructing a convex polygon with a basic set of vertices.
 */
TEST(ConvexPolygon, Construct) {
	std::vector<Point2> vertices = {Point2(10.1, 20.2), Point2(-40.4, 30.3), Point2(0, 20.2)};
	const ConvexPolygon poly(vertices);
	EXPECT_EQ(poly.get_vertices(), vertices) << "The vertices in the convex polygon must be the same as the input.";
}

/*!
 * Tests the copy constructor.
 */
TEST(ConvexPolygon, ConstructCopy) {
	std::vector<Point2> vertices = {Point2(10.1, 20.2), Point2(-40.4, 30.3), Point2(0, 20.2)};
	const ConvexPolygon original(vertices);
	const ConvexPolygon copy(original);
	EXPECT_EQ(original.get_vertices(), copy.get_vertices()) << "The copy must be an exact copy.";
}

}