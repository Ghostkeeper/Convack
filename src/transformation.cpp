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

Transformation& Transformation::translate(const coordinate_t x, const coordinate_t y) {
	data[4] += x;
	data[5] += y;
	return *this;
}

}