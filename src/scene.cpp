/*
 * Library to pack convex polygons into arbitrary shapes.
 * Any copyright is dedicated to the public domain. See LICENSE.md for more details.
 */

#include <queue> //A priority queue for beam search.
#include <utility> //For std::pair.

#include "beam/packing_candidate.hpp" //To track the best candidates for a beam search of packing orders.
#include "convex_polygon.hpp" //To handle the convex polygons as they are being packed.
#include "scene.hpp" //The definitions of the implementation defined here.

namespace convack {

class Scene::Impl {
public:
	/*! @copydoc Scene::pack(std::vector<ConvexPolygon>&) const
	 */
	void pack(std::vector<ConvexPolygon>& convex_polygons) const {
		//Choose which algorithm to use. In this case we only have one so far, but we'd like to keep the architecture open to more.
		pack_beam_search(convex_polygons);
	}

private:
	/*!
	 * Compares entries in the `best_orders` priority queue of the beam search.
	 *
	 * The entry with the lowest score is considered the best entry.
	 */
	struct LowestScore {
		bool operator ()(const PackingCandidate& a, const PackingCandidate& b) {
			return a.get_score() > b.get_score();
		}
	};

	/*!
	 * A packing algorithm that uses beam searching to find the optimal order to
	 * pack objects in.
	 *
	 * Objects are packed one by one when using this algorithm. The order in
	 * which to pack them is optimised with a beam search. That means that it
	 * considers a tree structure of all possible orders in which to pack
	 * objects.
	 * \param convex_polygons A list of convex polygons to pack. The result will
	 * be stored in this same list.
	 */
	void pack_beam_search(std::vector<ConvexPolygon>& convex_polygons) const {
		//Priority queue with the N best options to consider so far.
		std::priority_queue<PackingCandidate, std::vector<PackingCandidate>, LowestScore> best_orders;

		PackingCandidate root(convex_polygons, convex_polygons[0], nullptr);
	}
};

Scene::Scene() : pimpl(new Impl()) {}

void Scene::pack(std::vector<ConvexPolygon>& convex_polygons) const {
	pimpl->pack(convex_polygons);
}

}