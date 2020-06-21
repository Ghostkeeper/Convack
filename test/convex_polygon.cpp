/*
 * Library to pack convex polygons into arbitrary shapes.
 * Any copyright is dedicated to the public domain. See LICENSE.md for more details.
 */

#include <algorithm> //For std::shuffle.
#include <gtest/gtest.h> //To run the test.
#include <random> //For fuzz testing.
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
	 * 100 vertices in a long line.
	 */
	std::vector<Point2> colinear;

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

		colinear.clear();
		for(size_t i = 0; i < 100; ++i) {
			colinear.emplace_back(1.1 * i, 2.2 * i);
		}
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
 * Test equality when the vertex data is exactly the same.
 */
TEST_F(ConvexPolygonFixture, EqualityEqual) {
	const ConvexPolygon a(triangle);
	const ConvexPolygon b(triangle);

	EXPECT_EQ(a, b) << "The two convex polygons contain the same vertex data.";
}

/*!
 * Test equality when the vertex data has different sizes.
 */
TEST_F(ConvexPolygonFixture, EqualityDifferentSize) {
	const ConvexPolygon a(triangle); //3 vertices.
	const ConvexPolygon b(star); //8 vertices.

	EXPECT_NE(a, b) << "The two convex polygons have different numbers of vertices, so they are not equal.";
}

/*!
 * Tests that two convex polygons are different if they cover different areas.
 */
TEST_F(ConvexPolygonFixture, EqualityDifferentArea) {
	const ConvexPolygon a(triangle);
	const ConvexPolygon b({Point2(3, 0), Point2(4, 0), Point2(3.5, 10)}); //A completely different triangle.

	EXPECT_NE(a, b) << "The two convex polygons cover a different area, so they are not equal.";
}

/*!
 * Tests equality when two polygons are the same except their rotation.
 */
TEST_F(ConvexPolygonFixture, EqualityRotation) {
	const ConvexPolygon a(star);
	const ConvexPolygon b({
		star[3],
		star[4],
		star[5],
		star[6],
		star[7],
		star[0],
		star[1],
		star[2]
	});

	EXPECT_EQ(a, b) << "The two convex polygons cover the same area, even though the loop starts in a different spot along the contour.";
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

	EXPECT_EQ(result, ConvexPolygon(line)) << "With only two input vertices, the result must encompass both of these vertices and is just as degenerate.";
}

/*!
 * Tests getting the convex hull around a triangle, which is already convex.
 */
TEST_F(ConvexPolygonFixture, ConvexHullTriangle) {
	const ConvexPolygon result = ConvexPolygon::convex_hull(triangle);

	EXPECT_EQ(result, ConvexPolygon(triangle)) << "The triangle was already convex, so it must remain unchanged.";
}

/*!
 * Tests getting the convex hull around a star shape, which is concave.
 *
 * The convex hull should have removed the concave vertices.
 */
TEST_F(ConvexPolygonFixture, ConvexHullStar) {
	const ConvexPolygon result = ConvexPolygon::convex_hull(star);

	const ConvexPolygon ground_truth({
		Point2(100.0, 0.0),
		Point2(0.0, 100.0),
		Point2(-100.0, 0.0),
		Point2(0.0, -100.0)
	});
	EXPECT_EQ(result, ground_truth) << "Four vertices were concave and should have been left out. Only the tips of the star are left.";
}

/*!
 * Tests getting the convex hull around a triangle that is in reverse order.
 *
 * Reverse order means that the triangle represents a hole. The convex hull must
 * be a positive shape, so this effectively reverses the winding order of the
 * polygon.
 */
TEST_F(ConvexPolygonFixture, ConvexHullTriangleReversed) {
	const std::vector<Point2> inverse_triangle = {
		triangle[0],
		triangle[2], //Swapping vertices 1 and 2 results in a negative shape.
		triangle[1]
	};
	const ConvexPolygon result = ConvexPolygon::convex_hull(inverse_triangle);
	EXPECT_EQ(result, triangle) << "Taking the convex hull must always result in a positive shape.";
}

/*!
 * Tests taking the convex hull of a shape with many colinear line segments.
 *
 * All of the colinear points must've been filtered out. The convex hull must be
 * as efficient as possible.
 *
 * This tests the edge case where the vertices are all in order.
 */
TEST_F(ConvexPolygonFixture, ConvexHullColinearForwardOrder) {
	const ConvexPolygon result = ConvexPolygon::convex_hull(colinear);
	const ConvexPolygon ground_truth({Point2(0.0, 0.0), Point2(1.1 * 99, 2.2 * 99)});
	EXPECT_EQ(result, ground_truth);
}

}