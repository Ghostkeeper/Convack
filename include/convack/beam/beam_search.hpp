/*
 * Library to pack convex polygons into arbitrary shapes.
 * Any copyright is dedicated to the public domain. See LICENSE.md for more details.
 */

#ifndef CONVACK_BEAM_SEARCH
#define CONVACK_BEAM_SEARCH

#include <vector> //To process a list of convex polygons.

namespace convack {

class ConvexPolygon;
class PackingCandidate;
class Scene;

/*!
 * Class that implements the beam searching algorithm to pack convex polygons.
 *
 * Objects are packed one by one when using this algorithm. The order in which
 * to pack them is optimised with a beam search. That means that it considers a
 * tree structure of all possible orders in which to pack objects. At any point
 * it keeps track of a number of the most optimal candidates so far, and
 * explores those candidates further.
 */
class BeamSearch {
public:
	/*!
	 * Packs a number of convex polygons in a scene using beam search.
	 * \param scene The scene to pack them in, including obstacles and settings
	 * to use for the packing.
	 * \param convex_polygons The convex polygons to pack. The convex polygons
	 * are packed in-place by adjusting their transformations. The result will
	 * be stored in this same list of convex polygons.
	 */
	static void pack(const Scene& scene, std::vector<ConvexPolygon>& convex_polygons);

private:
	/*!
	 * Compares entries in the `best_orders` priority queue of the beam search.
	 *
	 * The entry with the lowest score is considered the best entry.
	 */
	struct LowestScore {
		/*!
		 * Executes the comparison operator.
		 * \param a The left-hand side candidate to compare.
		 * \param b The right-hand side candidate to compare.
		 * \return Whether the left-hand side is considered better than the
		 * right-hand side.
		 */
		bool operator ()(const PackingCandidate& a, const PackingCandidate& b);
	};
};

}

#endif