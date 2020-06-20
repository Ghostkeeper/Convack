/*
 * Library to pack convex polygons into arbitrary shapes.
 * Any copyright is dedicated to the public domain. See LICENSE.md for more details.
 */

#ifndef CONVACK_POINT2
#define CONVACK_POINT2

#include "coordinate.hpp" //To store the coordinates of this point.

namespace convack {

/*!
 * This data structure represents a point in 2D space.
 *
 * The first axis of this 2D space is termed "X", and the second axis is termed
 * "Y", by convention.
 */
struct Point2 {
	/*!
	 * Constructs a new 2D point located at the given coordinates.
	 * \param x The coordinate of this point on the X axis.
	 * \param y The coordinate of this point on the Y axis.
	 */
	Point2(const coordinate_t x, const coordinate_t y);

	/*!
	 * The coordinate of this point on the X axis.
	 */
	coordinate_t x;

	/*!
	 * The coordinate of this point on the Y axis.
	 */
	coordinate_t y;
};

}

#endif