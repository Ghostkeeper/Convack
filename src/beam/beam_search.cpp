/*
 * Library to pack convex polygons into arbitrary shapes.
 * Any copyright is dedicated to the public domain. See LICENSE.md for more details.
 */

#include <queue> //A priority queue to track the most optimal solutions in the beam search.

#include "beam/beam_search.hpp" //The definitions we're implementing here.
#include "beam/packing_candidate.hpp" //To create and track candidates of solutions that we'd like to explore.

namespace convack {

void BeamSearch::pack(const Scene& scene, std::vector<ConvexPolygon>& convex_polygons) {
	//Priority queue with the N best options to consider so far.
	std::priority_queue<PackingCandidate, std::vector<PackingCandidate>, LowestScore> best_orders;

	PackingCandidate root(convex_polygons, convex_polygons[0], nullptr);
}

bool BeamSearch::LowestScore::operator ()(const PackingCandidate& a, const PackingCandidate& b) {
	return a.get_score() > b.get_score();
}

}