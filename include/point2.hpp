/*
 * Library to pack convex polygons into arbitrary shapes.
 * Any copyright is dedicated to the public domain. See LICENSE.md for more details.
 */

#ifndef CONVACK_POINT2
#define CONVACK_POINT2

#include <ostream> //To be able to serialise a point to a stream.

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
	 * The coordinate of this point on the X axis.
	 */
	coordinate_t x;

	/*!
	 * The coordinate of this point on the Y axis.
	 */
	coordinate_t y;

	/*!
	 * Constructs a new 2D point located at the given coordinates.
	 * \param x The coordinate of this point on the X axis.
	 * \param y The coordinate of this point on the Y axis.
	 */
	Point2(const coordinate_t x, const coordinate_t y);

	/*!
	 * Compares if two points denote the same position in 2D space.
	 * \param other The 2D point to compare to.
	 * \return Whether the two points are the same (`true`) or different
	 * (`false`).
	 */
	bool operator ==(const Point2& other) const;

	/*!
	 * Compares if two points denote different positions in 2D space.
	 * \param other The 2D point to compare to.
	 * \return Whether the two points are different (`true`) or the same
	 * (`false`).
	 */
	bool operator !=(const Point2& other) const;

	/*!
	 * Overloads streaming this point.
	 *
	 * This is useful for debugging, since it allows printing the point to a
	 * stream directly, giving you a reasonably readable output.
	 */
	friend std::ostream& operator <<(std::ostream& output_stream, const Point2& point);
};

}

#endif