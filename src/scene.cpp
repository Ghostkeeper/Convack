//Library to pack convex polygons into arbitrary shapes.
//Any copyright is dedicated to the public domain. See LICENSE.md for more details.

#include "scene.h"

namespace convack {

class Scene::Impl {

};

Scene::Scene() : pimpl(new Impl()) {}

}