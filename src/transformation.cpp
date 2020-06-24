/*
 * Library to pack convex polygons into arbitrary shapes.
 * Any copyright is dedicated to the public domain. See LICENSE.md for more details.
 */

#include "transformation.hpp"

namespace convack {

Transformation::Transformation() {
	//Initialise as an identity matrix:
	// 1 0 0
	// 0 1 0
	data = {1, 0, 0, 1, 0, 0};
}

}