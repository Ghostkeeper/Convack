/*
 * Library to pack convex polygons into arbitrary shapes.
 * Any copyright is dedicated to the public domain. See LICENSE.md for more details.
 */

#include "beam/packing_candidate.hpp" //The definitions we're implementing here.
#include "convex_polygon.hpp" //To store some convex polygons and perform operations on them.

namespace convack {

PackingCandidate::PackingCandidate(const std::vector<ConvexPolygon>& packed_objects, const ConvexPolygon& pack_here, const PackingCandidate* const parent) :
		packed_objects(packed_objects),
		pack_here(pack_here),
		parent(parent) {
	score = compute_score();
}

double PackingCandidate::get_score() const {
	return score;
}

double PackingCandidate::compute_score() const {
	//Score is the ratio of area that is "lost" when packing objects this way.
	//The "lost" area is the part that is in the convex hull around all objects, but not covered by an object itself.

	//Serialise to a vector, because we need to provide a vector for the ConvexPolygon::convex_hull() call below.
	std::vector<ConvexPolygon> packed_so_far;
	const PackingCandidate* candidate = this;
	while(candidate) {
		packed_so_far.push_back(candidate->pack_here); //TODO: Makes an unnecessary copy!
		candidate = candidate->parent;
	}

	area_t covered_area = 0; //Original area.
	for(const ConvexPolygon& convex_polygon : packed_objects) {
		covered_area = convex_polygon.area();
	}

	area_t lost_area = ConvexPolygon::convex_hull(packed_objects).area(); //Will ALWAYS be bigger or equally big as the covered area.
	if(lost_area <= 0) {
		return 0; //Prevent division by 0.
	}

	return 1.0 - (covered_area / lost_area);
}

}