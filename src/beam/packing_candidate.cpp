/*
 * Library to pack convex polygons into arbitrary shapes.
 * Any copyright is dedicated to the public domain. See LICENSE.md for more details.
 */

#include "beam/packing_candidate.hpp" //The definitions we're implementing here.

namespace convack {

PackingCandidate::PackingCandidate(const std::vector<ConvexPolygon*>& packed_objects) : packed_objects(packed_objects) {
	score = 0; //TODO: Compute score.
}

}