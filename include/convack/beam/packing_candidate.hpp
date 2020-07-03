/*
 * Library to pack convex polygons into arbitrary shapes.
 * Any copyright is dedicated to the public domain. See LICENSE.md for more details.
 */

#ifndef CONVACK_PACKING_CANDIDATE
#define CONVACK_PACKING_CANDIDATE

#include <vector> //To store the convex polygons being packed so far.

#include "convex_polygon.hpp"

namespace convack {

/*!
 * This is a data structure that represents an intermediary state of convex
 * polygon packing.
 *
 * This effectively functions as a node in the search tree that the beam search
 * algorithm will search through. The tree is never generated completely (as
 * that would be `O(N*N!)` in computation and memory usage) but this candidate
 * can compute for itself what its child nodes are.
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
	 * \param packed_objects All of the objects that need to get packed. This
	 * includes objects that have already been packed before it reaches this
	 * candidate.
	 * \param pack_here The polygon that is new in the packing for this
	 * candidate. Should be one of the convex polygons from the
	 * \ref packed_objects vector.
	 * \param parent The candidate that this candidate is derived from, if any.
	 * You can see this as the parent node in the search tree. It contains a
	 * packing that does not contain the pack_here polygon yet.
	 */
	PackingCandidate(const std::vector<ConvexPolygon>& packed_objects, const ConvexPolygon& pack_here, const PackingCandidate* const parent);

	/*!
	 * Get the score of this candidate.
	 */
	double get_score() const;

private:
	/*!
	 * A list of all convex polygons that need to be packed.
	 *
	 * This includes objects that have already been packed in parent candidates,
	 * the object packed in this candidate as well as objects that have not yet
	 * been packed.
	 */
	const std::vector<ConvexPolygon>& packed_objects;

	/*!
	 * This candidate builds upon its parent candidates by adding one new convex
	 * polygon to its packing. The new convex polygon is stored in this node,
	 * while the previously packed objects are stored in the parent nodes.
	 */
	ConvexPolygon pack_here;

	/*!
	 * A reference to the parent candidate upon which this candidate extends the
	 * search.
	 *
	 * If there is no parent candidate, this will be a `nullptr`.
	 */
	const PackingCandidate* const parent;

	/*!
	 * How well this candidate is rated. A lower score is considered a better
	 * choice.
	 *
	 * The score is independent of which measure is used to optimise the
	 * packing.
	 */
	double score;

	/*!
	 * Compute the score for this candidate.
	 */
	double compute_score() const;
};

}

#endif

