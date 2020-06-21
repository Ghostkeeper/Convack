/*
 * Library to pack convex polygons into arbitrary shapes.
 * Any copyright is dedicated to the public domain. See LICENSE.md for more details.
 */

#include <gtest/gtest.h> //To run the test.
#include <vector> //To store vertices of test polygons.

#include "convex_polygon.hpp" //The unit under test.

namespace convack {

/*!
 * Fixture with some base test cases to test the ConvexPolygon class with.
 */
class ConvexPolygonFixture : public testing::Test {
public:
	/*!
	 * A four-pointed star, centred around 0,0.
	 *
	 * This shape is not convex.
	 */
	std::vector<Point2> star;

	/*!
	 * A triangle (which is always convex).
	 */
	std::vector<Point2> triangle;

	/*!
	 * Prepare to run a test. This creates the fixture members.
	 */
	void SetUp() {
		star.assign({
			Point2(100.0, 0.0),
			Point2(20.0, 20.0),
			Point2(0.0, 100.0),
			Point2(-20.0, 20.0),
			Point2(-100.0, 0.0),
			Point2(-20.0, -20.0),
			Point2(0.0, -100.0),
			Point2(20.0, -20.0)
		});
		triangle.assign({
			Point2(0.0, 0.0),
			Point2(50.0, 0.0),
			Point2(25.0, 50.0)
		});
	}
};

/*!
 * Test constructing a convex polygon from a predefined set of vertices.
 */
TEST(ConvexPolygon, ConstructEmpty) {
	const std::vector<Point2> empty;
	const ConvexPolygon poly(empty);
	EXPECT_TRUE(poly.get_vertices().empty()) << "There were no vertices in the input.";
}

/*!
 * Tests constructing a convex polygon with a basic set of vertices.
 */
TEST_F(ConvexPolygonFixture, Construct) {
	const ConvexPolygon poly(star);
	EXPECT_EQ(poly.get_vertices(), star) << "The vertices in the convex polygon must be the same as the input.";
}

/*!
 * Tests the copy constructor.
 */
TEST_F(ConvexPolygonFixture, ConstructCopy) {
	const ConvexPolygon original(star);
	const ConvexPolygon copy(star);
	EXPECT_EQ(original.get_vertices(), copy.get_vertices()) << "The copy must be an exact copy.";
}

/*!
 * Test equality of two empty convex polygons.
 */
TEST(ConvexPolygon, EqualityEmpty) {
	const ConvexPolygon a({});
	const ConvexPolygon b({});

	EXPECT_EQ(a, b) << "The two convex polygons are both empty, and thus equal.";
}

/*!
 * Tests getting the convex hull of an empty set of vertices.
 */
TEST(ConvexPolygon, ConvexHullEmpty) {
	const std::vector<Point2> empty;
	const ConvexPolygon result = ConvexPolygon::convex_hull(empty);

	EXPECT_TRUE(result.get_vertices().empty()) << "With no input vertices, the convex hull is just as degenerate.";
}

/*!
 * Tests getting the convex hull of a single vertex.
 */
TEST(ConvexPolygon, ConvexHullSingle) {
	const std::vector<Point2> single = {Point2(10.0, 10.0)};
	const ConvexPolygon result = ConvexPolygon::convex_hull(single);

	EXPECT_EQ(result.get_vertices(), single) << "With just one input vertex, the convex hull contains just the one vertex and is just as degenerate.";
}

/*!
 * Tests getting the convex hull around a line.
 */
TEST(ConvexPolygon, ConvexHullLine) {
	const std::vector<Point2> line = {Point2(10.0, 10.0), Point2(20.0, 20.0)};
	const ConvexPolygon result = ConvexPolygon::convex_hull(line);

	EXPECT_EQ(result.get_vertices(), line) << "With only two input vertices, the result must encompass both of these vertices and is just as degenerate.";
}

}