/*
 * Library to pack convex polygons into arbitrary shapes.
 * Any copyright is dedicated to the public domain. See LICENSE.md for more details.
 */

#ifndef CONVACK_CONVEX_POLYGON
#define CONVACK_CONVEX_POLYGON

#include <memory> //For unique_ptr.
#include <vector> //To store the coordinates of this convex polygon.

#include "point2.hpp" //The vertices of this convex polygon.

namespace convack {

/*!
 * Data structure representing one convex polygon.
 *
 * The class by itself doesn't enforce that it's convex, but does assume that it
 * is. It's the duty of the consumer of this class to ensure that it is convex.
 * Alternatively, it also provides a method to create a convex polygon enclosing
 * a number of given coordinates.
 *
 * This class contains a number of algorithms operating on convex polygons. Due
 * to the nature of the convexity constraint, many algorithms become
 * considerably simpler and faster when working only with convex polygons.
 */
class ConvexPolygon {
public:
	/*!
	 * Constructs a new convex polygon using the provided vertices.
	 *
	 * The input vertices are assumed to be already convex and in the correct
	 * order. In order to improve performance, the input is not checked for
	 * convexity.
	 * \param vertices The vertices of a convex polygon.
	 */
	ConvexPolygon(const std::vector<Point2>& vertices);

	/*!
	 * Deletes the convex polygon.
	 */
	~ConvexPolygon();

	/*!
	 * Get the vertices of the convex hull.
	 */
	const std::vector<Point2>& get_vertices() const;

private:
	/*!
	 * The implementation of the convex polygon is separated into this class.
	 *
	 * This implements the PIMPL idiom.
	 *
	 * This allows consumers of this library to use newer versions of Convack
	 * than what they compiled their application for. They can install bug fixes
	 * without recompiling their application, even if the data stored in the
	 * implementation changes. Only for backwards compatibility changes will
	 * they need to recompile their application.
	 */
	class Impl;

	/*!
	 * A pointer to the implementation of this class.
	 *
	 * This implements the PIMPL idiom.
	 */
	std::unique_ptr<Impl> pimpl;
};

}

#endif