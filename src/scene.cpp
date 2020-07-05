/*
 * Library to pack convex polygons into arbitrary shapes.
 * Any copyright is dedicated to the public domain. See LICENSE.md for more details.
 */

#include "beam/beam_search.hpp" //To pack polyons using the beam searching algorithm.
#include "scene.hpp" //The definitions of the implementation defined here.

namespace convack {

class ConvexPolygon;

class Scene::Impl {
public:
	/*!
	 * Construct the implementation.
	 * \param scene A reference to the `Scene` that this instance is the
	 * implementation of.
	 */
	Impl(Scene& scene) :
			scene(scene),
			beam_width(10) {
	}

	/*! @copydoc Scene::pack(std::vector<ConvexPolygon>&) const
	 */
	void pack(std::vector<ConvexPolygon>& convex_polygons) const {
		//Choose which algorithm to use. In this case we only have one so far, but we'd like to keep the architecture open to more.
		BeamSearch::pack(scene, convex_polygons);
	}

	/*! @copydoc Scene::set_beam_width(const size_t)
	 */
	void set_beam_width(const size_t new_beam_width) {
		beam_width = new_beam_width;
	}

	/*! @copydoc Scene::get_beam_width() const
	 */
	size_t get_beam_width() const {
		return beam_width;
	}

private:
	/*!
	 * How wide the beam search is searching through sub-optimal choices.
	 *
	 * With a greater beam width, the beam search algorithm will consider more
	 * sub-optimal packing options during intermediate stages of the search
	 * process. This way, the beam search can find packings that are more
	 * optimal in the end, but require an intermediary result that is less
	 * optimal. In effect it will find a global optimum more easily rather than
	 * getting stuck in a local optimum.
	 *
	 * A greater beam width will also cause the algorithm to take more
	 * processing power.
	 *
	 * The beam width must be at least 1. A beam width of 1 will turn this into
	 * a completely greedy search, adding the best object at each stage but not
	 * exploring any suboptimal choices.
	 */
	size_t beam_width;

	/*!
	 * A reference to the encapsulating public interface. The `Scene`.
	 *
	 * This is used to pass on the encapsulating `Scene` instance as parameter
	 * to functions that might need it.
	 */
	Scene& scene;
};

Scene::Scene() : pimpl(new Impl(*this)) {}

Scene::~Scene() = default;

void Scene::pack(std::vector<ConvexPolygon>& convex_polygons) const {
	pimpl->pack(convex_polygons);
}

void Scene::set_beam_width(const size_t new_beam_width) {
	pimpl->set_beam_width(new_beam_width);
}

size_t Scene::get_beam_width() const {
	return pimpl->get_beam_width();
}

}