/*
 * Library to pack convex polygons into arbitrary shapes.
 * Any copyright is dedicated to the public domain. See LICENSE.md for more details.
 */

#include "point2.hpp"

namespace convack {

Point2::Point2(const coordinate_t x, const coordinate_t y) : x(x), y(y) {}

bool Point2::operator ==(const Point2& other) const {
	return x == other.x && y == other.y;
}

}