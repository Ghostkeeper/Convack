/*
 * Library to pack convex polygons into arbitrary shapes.
 * Any copyright is dedicated to the public domain. See LICENSE.md for more details.
 */

#include "scene.hpp" //The definitions of the implementation defined here.

namespace convack {

class Scene::Impl {
public:
	void pack(std::vector<ConvexPolygon>& convex_polygons) const {
		//TODO.
	}
};

Scene::Scene() : pimpl(new Impl()) {}

void Scene::pack(std::vector<ConvexPolygon>& convex_polygons) const {
	pimpl->pack(convex_polygons);
}

}