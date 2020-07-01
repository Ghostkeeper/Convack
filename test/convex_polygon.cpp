/*
 * Library to pack convex polygons into arbitrary shapes.
 * Any copyright is dedicated to the public domain. See LICENSE.md for more details.
 */

#include <algorithm> //For std::shuffle.
#include <cmath> //To calculate correct rotation matrices.
#include <gtest/gtest.h> //To run the test.
#include <random> //For fuzz testing.
#include <vector> //To store vertices of test polygons.

#include "convex_polygon.hpp" //The unit under test.
#include "point2.hpp" //To construct convex polygons and vertices for testing.
#include "transformation.hpp" //To test tracking of transformations.

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
	 * A 100-sided regular polygon, approximating a circle of radius 1.
	 */
	std::vector<Point2> circle;

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

		circle.clear();
		constexpr double pi = std::acos(-1);
		for(size_t i = 0; i < 100; ++i) {
			const double angle = pi * 2 / 100 * i; //Angle in radians.
			circle.emplace_back(std::cos(angle), std::sin(angle));
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

/*!
 * Tests taking the convex hull of a shape with many colinear line segments.
 *
 * All of the colinear points must've been filtered out. The convex hull must be
 * as efficient as possible.
 *
 * This tests the edge case where the vertices are all in reverse order.
 */
TEST_F(ConvexPolygonFixture, ConvexHullColinearBackwardOrder) {
	std::reverse(colinear.begin(), colinear.end());
	const ConvexPolygon result = ConvexPolygon::convex_hull(colinear);
	const ConvexPolygon ground_truth({Point2(0.0, 0.0), Point2(1.1 * 99, 2.2 * 99)});
	EXPECT_EQ(result, ground_truth);
}

/*!
 * Tests taking the convex hull of a shape with many colinear line segments.
 *
 * All of the colinear points must've been filtered out. The convex hull must be
 * as efficient as possible.
 *
 * This is a fuzz test. It'll randomise the list of vertices before taking the
 * convex hull, many times. The random number generator is seeded, so the test
 * is still deterministic. This should catch most cases of bugs in the colinear
 * case.
 */
TEST_F(ConvexPolygonFixture, ConvexHullColinearRandomOrder) {
	constexpr int num_shuffle = 1000; //How often to repeat the test with random order. Increase to catch more cases, but slower tests.
	std::default_random_engine randomiser{42}; //Use a fixed seed so the tests are deterministic.
	for(size_t i = 0; i < num_shuffle; ++i) {
		std::shuffle(colinear.begin(), colinear.end(), randomiser); //Use a fixed seed so the tests are deterministic.
		const ConvexPolygon result = ConvexPolygon::convex_hull(colinear);
		const ConvexPolygon ground_truth({Point2(0.0, 0.0), Point2(1.1 * 99, 2.2 * 99)});
		EXPECT_EQ(result, ground_truth);
	}
}

/*!
 * Tests taking the convex hull of a shape where all points overlay one another.
 *
 * Kind of an extreme edge case.
 */
TEST(ConvexPolygon, ConvexHullSinglePoint) {
	std::vector<Point2> single_point;
	for(size_t i = 0; i < 10; ++i) {
		single_point.emplace_back(42, 69);
	}

	const ConvexPolygon result = ConvexPolygon::convex_hull(single_point);

	ASSERT_EQ(result.get_vertices().size(), 1) << "All of the extraneous points collapse to 1.";
	EXPECT_EQ(result.get_vertices()[0], Point2(42, 69)) << "The coordinates for all of the points were this. That must be retained.";
}

/*!
 * Test taking the convex hull of an empty set of convex polygons.
 */
TEST(ConvexPolygon, ConvexPolyHullEmpty) {
	const std::vector<ConvexPolygon> empty;
	EXPECT_EQ(ConvexPolygon::convex_hull(empty), ConvexPolygon({})) << "There was no input data, so the convex hull is empty.";
}

/*!
 * Test taking the convex hull around a single convex polygon.
 */
TEST_F(ConvexPolygonFixture, ConvexPolyHullSingle) {
	const std::vector<ConvexPolygon> single = {ConvexPolygon(triangle)};
	EXPECT_EQ(ConvexPolygon::convex_hull(single), ConvexPolygon(triangle)) << "The input data is already convex, so taking the convex hull results in the same polygon.";
}

/*!
 * Test taking the convex hull around two triangles, offset from each other.
 */
TEST_F(ConvexPolygonFixture, ConvexPolyHullTwoTriangles) {
	const std::vector<ConvexPolygon> two {
		ConvexPolygon(triangle),
		ConvexPolygon(triangle).translate(100, 10)
	};
	const ConvexPolygon ground_truth({
		Point2(0, 0), //Far left corner of the leftmost polygon.
		Point2(50, 0), //Lower right corner of the leftmost polygon.
		Point2(150, 10), //Jumps to the second polygon here. Far right corner of the rightmost polygon.
		Point2(125, 60), //Tip of the rightmost polygon.
		Point2(25, 50), //Jumps back to the first polygon. Tip of the leftmost polygon.
	});
	EXPECT_EQ(ConvexPolygon::convex_hull(two), ground_truth);
}

/*!
 * Test taking the convex hull around two circles, offset from each other.
 *
 * This essentially tests how it works with more complex shapes, with more than
 * 3 vertices.
 */
TEST_F(ConvexPolygonFixture, ConvexPolyHullTwoCircles) {
	const std::vector<ConvexPolygon> pair { //Input circles.
		ConvexPolygon(circle),
		ConvexPolygon(circle).translate(100, 0)
	};

	std::vector<Point2> truth_vertices; //Construct the ground truth.
	truth_vertices.reserve(circle.size() + 2);
	for(size_t i = 0; i <= circle.size() / 4; ++i) { //Take the first quarter circle. This is part of the right-hand side of the convex hull. This is correct iff circle.size() is divisible by 4.
		truth_vertices.emplace_back(circle[i].x + 100, circle[i].y);
	}
	for(size_t i = circle.size() / 4; i <= circle.size() * 3 / 4; ++i) { //The next half is on the left-hand side.
		truth_vertices.push_back(circle[i]);
	}
	for(size_t i = circle.size() * 3 / 4; i < circle.size(); ++i) { //And the last quarter is on the right-hand side again.
		truth_vertices.emplace_back(circle[i].x + 100, circle[i].y);
	}
	const ConvexPolygon ground_truth(truth_vertices);

	EXPECT_EQ(ConvexPolygon::convex_hull(pair), ground_truth);
}

/*!
 * Test taking the convex hull around four triangles.
 *
 * This tests how it deals with more than 2 shapes.
 */
TEST_F(ConvexPolygonFixture, ConvexPolyHullFourTriangles) {
	const std::vector<ConvexPolygon> four {
		ConvexPolygon(triangle),
		ConvexPolygon(triangle).translate(100, -10),
		ConvexPolygon(triangle).translate(100, 60),
		ConvexPolygon(triangle).translate(0, 60)
	};
	const ConvexPolygon ground_truth({
		Point2(0, 0), //Lower left corner of the bottom left polygon.
		Point2(100, -10), //Lower left corner of the bottom right polygon (since it's lower than the bottom right corner of the bottom left polygon).
		Point2(150, -10), //Lower right corner of the bottom right polygon.
		Point2(150, 60), //Lower right corner of the top right polygon.
		Point2(125, 110), //Top corner of the top right polygon.
		Point2(25, 110), //Top corner of the top left polygon.
		Point2(0, 60) //Lower left corner of the top left polygon.
	});

	EXPECT_EQ(ConvexPolygon::convex_hull(four), ground_truth);
}

/*!
 * Test taking the convex hull of shapes that overlap.
 */
TEST_F(ConvexPolygonFixture, ConvexPolyHullOverlapping) {
	const std::vector<ConvexPolygon> pair {
		ConvexPolygon(triangle),
		ConvexPolygon(triangle).translate(25, 25) //Translated such that the left-bottom corner falls smack in the middle of the other convex polygon.
	};
	const ConvexPolygon ground_truth({
		Point2(0, 0), //Lower left corner of the first polygon.
		Point2(50, 0), //Lower right corner of the first polygon.
		Point2(75, 25), //Lower right corner of the second polygon.
		Point2(50, 75), //Top corner of the second polygon.
		Point2(25, 50) //Top corner of the first polygon.
	});

	EXPECT_EQ(ConvexPolygon::convex_hull(pair), ground_truth);
}

/*!
 * Test computing the area of an empty convex polygon.
 */
TEST(ConvexPolygon, AreaEmpty) {
	const ConvexPolygon polygon({});
	EXPECT_EQ(polygon.area(), 0) << "Empty convex polygons have no area.";
}

/*!
 * Test computing the area of a convex polygon with a single vertex.
 */
TEST(ConvexPolygon, AreaSingle) {
	const ConvexPolygon polygon({Point2(3.14, 6.28)});
	EXPECT_EQ(polygon.area(), 0) << "A single vertex has no area.";
}

/*!
 * Test computing the area of a convex polygon with two vertices.
 */
TEST(ConvexPolygon, AreaLine) {
	const ConvexPolygon polygon({Point2(42, 69), Point2(69, 42)});
	EXPECT_EQ(polygon.area(), 0) << "A line has no surface area.";
}

/*!
 * Test computing the area of a simple triangle.
 */
TEST_F(ConvexPolygonFixture, AreaTriangle) {
	EXPECT_DOUBLE_EQ(ConvexPolygon(triangle).area(), 50.0 * 50.0 / 2) << "This is a triangle with base 50 and height 50.";
}

TEST_F(ConvexPolygonFixture, AreaCircle) {
	constexpr double pi = std::acos(-1);
	constexpr area_t ground_truth = 100.0 * std::sin(pi * 2 / 100) / 2; //Formula for area of a regular polygon (100 vertices, radius 1.0);
	EXPECT_NEAR(ConvexPolygon(circle).area(), ground_truth, 0.000005) << "This shape is a regular polygon with 100 sides and radius 1.";
}

/*!
 * Test collision between two empty convex polygons.
 */
TEST(ConvexPolygon, ColliesTwoEmpty) {
	const ConvexPolygon a({});
	const ConvexPolygon b({});
	EXPECT_FALSE(a.collides(b)) << "Empty convex polygons never collide with anything.";
	EXPECT_FALSE(b.collides(a)) << "The inverse always gives the same result.";
}

/*!
 * Test collision between an empty convex polygon and a normal convex polygon.
 */
TEST_F(ConvexPolygonFixture, CollidesEmpty) {
	const ConvexPolygon a({});
	EXPECT_FALSE(a.collides(ConvexPolygon(triangle))) << "An empty convex polygon has no area and so doesn't collide with anything at all.";
	EXPECT_FALSE(ConvexPolygon(triangle).collides(a)) << "The inverse always gives the same result.";
}

/*!
 * Test collision between a single vertex and a normal convex polygon.
 */
TEST_F(ConvexPolygonFixture, CollidesVertex) {
	const ConvexPolygon single_vertex_outside({Point2(1000, 1000)}); //This convex polygon has a single vertex far outside of the triangle to test against.
	EXPECT_FALSE(single_vertex_outside.collides(ConvexPolygon(triangle))) << "The vertex is far away from the convex polygon.";
	EXPECT_FALSE(ConvexPolygon(triangle).collides(single_vertex_outside)) << "The inverse always gives the same result.";

	const ConvexPolygon single_vertex_inside({Point2(25, 25)}); //A vertex inside of the triangle.
	EXPECT_FALSE(single_vertex_inside.collides(ConvexPolygon(triangle))) << "Even though the vertex is inside the other triangle, collision of the edges and vertices doesn't count.";
	EXPECT_FALSE(ConvexPolygon(triangle).collides(single_vertex_inside)) << "The inverse always gives the same result.";
}

/*!
 * Test collision between a line and a normal convex polygon.
 */
TEST_F(ConvexPolygonFixture, CollidesLine) {
	const ConvexPolygon line_outside({Point2(1000, 1000), Point2(1010, 1000)}); //A 2-vertex polygon very far away from the triangle.
	EXPECT_FALSE(line_outside.collides(ConvexPolygon(triangle))) << "The line is far away from the convex polygon.";
	EXPECT_FALSE(ConvexPolygon(triangle).collides(line_outside)) << "The inverse always gives the same result.";

	const ConvexPolygon line_through({Point2(0, 25), Point2(49, 25)}); //This line goes through the convex polygon. Neither of the vertices are inside.
	EXPECT_FALSE(line_through.collides(ConvexPolygon(triangle))) << "Although the line goes through the convex polygon, collision with only an edge of the line doesn't count.";
	EXPECT_FALSE(ConvexPolygon(triangle).collides(line_through)) << "The inverse always gives the same result.";

	const ConvexPolygon line_inside({Point2(10, 2), Point2(40, 2)}); //This line starts and ends in the convex polygon.
	EXPECT_FALSE(line_inside.collides(ConvexPolygon(triangle))) << "Although the line is completely inside the convex polygon, collision with only an edge of the line doesn't count.";
	EXPECT_FALSE(ConvexPolygon(triangle).collides(line_inside)) << "The inverse always gives the same result.";
}

/*!
 * Test collision between two convex polygons that are very far away from each
 * other.
 */
TEST_F(ConvexPolygonFixture, CollidesFar) {
	const ConvexPolygon a(triangle);
	ConvexPolygon b(triangle);
	b.translate(1000, 0);

	EXPECT_FALSE(a.collides(b)) << "The two convex polygons are very far away from each other, so they don't collide.";
	EXPECT_FALSE(b.collides(a)) << "The inverse always gives the same result.";
}

/*!
 * Test collision between two convex polygons that are very near to each other.
 *
 * In this test they are so close together that a simple bounding box check
 * would consider them possibly colliding.
 */
TEST_F(ConvexPolygonFixture, CollidesNear) {
	const ConvexPolygon a(triangle);
	ConvexPolygon b(triangle);
	b.translate(30, 45); //This brings the 0,0 lower left corner up to 30,45, which is just next to the upper middle corner of the other triangle.

	EXPECT_FALSE(a.collides(b)) << "The two convex polygons are close, but not colliding.";
	EXPECT_FALSE(b.collides(a)) << "The inverse always gives the same result.";
}

/*!
 * Test collision between two convex polygons that are touching with their
 * contours.
 *
 * This touch is not considered to be an actual collision, so the implementation
 * must report that they are not colliding.
 */
TEST_F(ConvexPolygonFixture, CollidesTouching) {
	const ConvexPolygon a(triangle);
	ConvexPolygon b(triangle);
	b.translate(30, 40); //This brings the 0,0 lower left corner up to 30,45, which is on the edge between the upper middle corner and the lower right corner of the triangle.

	EXPECT_FALSE(a.collides(b)) << "The edge case of a vertex touching an edge of the other convex polygon is not considered a collision.";
	EXPECT_FALSE(b.collides(a)) << "The inverse always gives the same result.";
}

/*!
 * Test collision between two convex polygons that actually overlap.
 */
TEST_F(ConvexPolygonFixture, CollidesOverlapping) {
	const ConvexPolygon a(triangle);
	ConvexPolygon b(triangle);
	b.translate(25, 25); //This brings the 0,0 lower left corner up to 25,25, right in the middle of the other triangle.

	EXPECT_TRUE(a.collides(b)) << "One of the vertices of B is inside A.";
	EXPECT_TRUE(b.collides(a)) << "The inverse always gives the same result.";
}

/*!
 * Test collision between two convex polygons where one polygon pierces the
 * other without actually having a vertex inside the other polygon.
 */
TEST_F(ConvexPolygonFixture, CollidesThrough) {
	const ConvexPolygon a(triangle);
	const ConvexPolygon b({
		Point2(0, 10),
		Point2(50, 10),
		Point2(50, 20),
		Point2(0, 20)
	}); //Forms a rectangle without any vertex inside the triangle, but where the edges cross the edges of the triangle.

	EXPECT_TRUE(a.collides(b)) << "B crosses through A.";
	EXPECT_TRUE(b.collides(a)) << "The inverse always gives the same result.";
}

/*!
 * Tests that an empty convex polygon never contains any points.
 */
TEST(ConvexPolygon, ContainsEmpty) {
	const ConvexPolygon empty({});
	EXPECT_FALSE(empty.contains(Point2(42, 69))) << "An empty convex polygon never contains anything.";
}

/*!
 * Tests that a convex polygon with a single vertex never contains any points.
 */
TEST(ConvexPolygon, ContainsSingleVertex) {
	const ConvexPolygon single({Point2(100, 200)});
	EXPECT_FALSE(single.contains(Point2(50, 100))) << "Different location, so definitely outside of the convex polygon.";
	EXPECT_FALSE(single.contains(Point2(100, 200))) << "Even though this location is the same as the one vertex of the polygon, it's still just on the border so it's not considered inside.";
}

/*!
 * Tests that a convex polygon with two vertices never contains any points.
 */
TEST(ConvexPolygon, ContainsLine) {
	const ConvexPolygon line({Point2(0, 0), Point2(100, 200)});
	EXPECT_FALSE(line.contains(Point2(10, 10))) << "This point is completely away from the line.";
	EXPECT_FALSE(line.contains(Point2(50, 100))) << "Even though this point is on the line, it's still just on the border so it's not considered inside.";
	EXPECT_FALSE(line.contains(Point2(0, 0))) << "Points on the vertices of the convex polygon are still considered outside.";
	EXPECT_FALSE(line.contains(Point2(100, 200))) << "Points on the vertices of the convex polygon are still considered outside.";
}

/*!
 * Tests whether a point inside a convex polygon is correctly identified as
 * inside.
 */
TEST_F(ConvexPolygonFixture, ContainsInside) {
	EXPECT_TRUE(ConvexPolygon(triangle).contains(Point2(25, 10))) << "This point is completely inside the triangle.";
}

/*!
 * Tests whether a point outside a convex polygon is correctly identified as
 * outside.
 */
TEST_F(ConvexPolygonFixture, ContainsOutside) {
	EXPECT_FALSE(ConvexPolygon(triangle).contains(Point2(100, 10))) << "This point is completely outside of the triangle.";
}

/*!
 * Tests whether a point on the border of a convex polygon is correctly
 * identified as outside.
 */
TEST_F(ConvexPolygonFixture, ContainsEdge) {
	const ConvexPolygon polygon(triangle);
	EXPECT_FALSE(polygon.contains(Point2(30, 0))) << "This point is on the lower border of the triangle, and the border is considered outside.";
	EXPECT_FALSE(polygon.contains(Point2(50, 0))) << "This point is on one of the vertices of the triangle, and the border is considered outside.";
	EXPECT_FALSE(polygon.contains(Point2(80, 0))) << "This point is aligned with the line through one of the edges, but is actually completely outside of the triangle.";
}

/*!
 * Test tracking the current transformation through multiple transformations.
 */
TEST_F(ConvexPolygonFixture, CurrentTransformation) {
	ConvexPolygon polygon(triangle);
	EXPECT_EQ(polygon.current_transformation(), Transformation()) << "The initial transformation of a convex polygon must be the identity transformation.";

	polygon.translate(11, 22);
	EXPECT_EQ(polygon.current_transformation(), Transformation().translate(11, 22)) << "When translating a convex polygon, the current transformation must be tracked.";

	polygon.rotate(3);
	EXPECT_EQ(polygon.current_transformation(), Transformation().translate(11, 22).rotate(3)) << "When rotating a convex polygon, the current transformation must be tracked.";

	polygon.translate(-11, -22);
	EXPECT_EQ(polygon.current_transformation(), Transformation().translate(11, 22).rotate(3).translate(-11, -22)) << "Multiple transformations must be stacked together and reported as one transformation.";
}

/*!
 * Test moving the convex polygon.
 */
TEST_F(ConvexPolygonFixture, Translate) {
	ConvexPolygon polygon(triangle);
	polygon.translate(42, 69);

	const ConvexPolygon ground_truth({
		Point2(42, 69),
		Point2(92, 69),
		Point2(67, 119)
	});
	EXPECT_EQ(polygon, ground_truth) << "The convex polygon was moved 42 towards positive X and 69 towards positive Y, so all vertices must be translated.";
}

/*!
 * Test rotating the convex polygon.
 */
TEST_F(ConvexPolygonFixture, Rotate) {
	ConvexPolygon polygon(triangle);
	constexpr double pi = std::acos(-1);
	polygon.rotate(pi / 2);

	const ConvexPolygon ground_truth({
		Point2(0, 0),
		Point2(0, 50),
		Point2(-50, 25)
	});

	//To test, we need to do approximate equality since rounding errors may occur.
	//This simple assertion will fail if the polygon's vertices are shifted in the array!
	//Technically that should not fail, since the polygon is still the same then and that is acceptable.
	//However this case is very unlikely considering any implementation of rotating the polygon.
	//So this is now a much simpler comparison vertex-by-vertex.
	EXPECT_EQ(polygon.get_vertices().size(), ground_truth.get_vertices().size()) << "Rotation doesn't create or destroy vertices.";
	for(size_t vertex = 0; vertex < polygon.get_vertices().size(); ++vertex) {
		EXPECT_NEAR(polygon.get_vertices()[vertex].x, ground_truth.get_vertices()[vertex].x, 0.0000001) << "The point must be rotated 1/4 turn around 0,0.";
		EXPECT_NEAR(polygon.get_vertices()[vertex].y, ground_truth.get_vertices()[vertex].y, 0.0000001) << "The point must be rotated 1/4 turn around 0,0.";
	}
}

}