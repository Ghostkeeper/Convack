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
	Impl(Scene& scene) : scene(scene) {}

	/*! @copydoc Scene::pack(std::vector<ConvexPolygon>&) const
	 */
	void pack(std::vector<ConvexPolygon>& convex_polygons) const {
		//Choose which algorithm to use. In this case we only have one so far, but we'd like to keep the architecture open to more.
		BeamSearch::pack(scene, convex_polygons);
	}

private:
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

}