/*
 * Library to pack convex polygons into arbitrary shapes.
 * Any copyright is dedicated to the public domain. See LICENSE.md for more details.
 */

#include <gtest/gtest.h> //To run the test.

#include "beam/packing_candidate.hpp" //The unit under test.
#include "convex_polygon.hpp" //To create polygons to test with.
#include "point2.hpp" //To create polygons to test with.

namespace convack {

/*!
 * A fixture containing some helpful objects to test on, such as convex
 * polygons.
 */
class PackingCandidateFixture : public testing::Test {
public:
	/*!
	 * A simple triangle shape.
	 */
	ConvexPolygon triangle;

	/*!
	 * This constructor initialises all fields necessary to empty.
	 *
	 * The fixtures are actually filled in SetUp, but this way the compiler can
	 * be assured that the fields are filled.
	 */
	PackingCandidateFixture() : triangle({}) { }

	/*!
	 * Executed before every test in order to create or reset the fixtures.
	 */
	void SetUp() {
		triangle = ConvexPolygon(std::vector<Point2>({
			Point2(0, 0),
			Point2(50, 0),
			Point2(25, 50),
		}));
	}
};

/*!
 * Test computing the score of a packing candidate which just needs to pack a
 * single object.
 */
TEST_F(PackingCandidateFixture, ComputeScoreSingle) {
	const std::vector<ConvexPolygon> packed_objects({triangle});
	PackingCandidate candidate(packed_objects, triangle, nullptr);

	EXPECT_EQ(candidate.get_score(), 0) << "Since there is only one object to pack, the packing is perfect.";
}

}