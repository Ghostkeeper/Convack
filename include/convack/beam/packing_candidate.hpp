/*
 * Library to pack convex polygons into arbitrary shapes.
 * Any copyright is dedicated to the public domain. See LICENSE.md for more details.
 */

#ifndef CONVACK_PACKING_CANDIDATE
#define CONVACK_PACKING_CANDIDATE

#include <vector> //To store the convex polygons being packed so far.

namespace convack {

class ConvexPolygon;

/*!
 * This is a data structure that represents an intermediary state of convex
 * polygon packing.
 *
 * The candidate has a fixed set of convex polygons that are packed so far. It
 * also has a score, determined by some measure of what is considered to be a
 * good packing. The score is computed upon construction.
 */
class PackingCandidate {
public:
	/*!
	 * Construct a new candidate with the given list of packed objects.
	 *
	 * The candidate will compute its score immediately. The list of packed
	 * objects is fixed for this candidate, so the packed objects or the score
	 * cannot change during the lifetime of this candidate.
	 */
	PackingCandidate(const std::vector<ConvexPolygon*>& packed_objects);

	/*!
	 * Get the score of this candidate.
	 */
	double get_score() const;

private:
	/*!
	 * A list of convex polygons that need to be packed.
	 */
	std::vector<ConvexPolygon*> packed_objects;

	/*!
	 * How well this candidate is rated. A lower score is considered a better
	 * choice.
	 *
	 * The score is independent of which measure is used to optimise the
	 * packing.
	 */
	double score;
};

}

#endif

