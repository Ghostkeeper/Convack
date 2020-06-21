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
	 * Constructs a convex hull around a number of points.
	 *
	 * You could provide the vertices of a concave polygon in any order, or just
	 * any set of loose points. The result is the same: A convex polygon that
	 * includes all of the given points.
	 * \param points The points to construct a convex hull around.
	 * \return A convex polygon around the given points.
	 */
	static ConvexPolygon convex_hull(const std::vector<Point2>& points);

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
	 * Copies a convex polygon.
	 * \param original The original convex polygon to copy.
	 */
	ConvexPolygon(const ConvexPolygon& original);

	/*!
	 * Deletes the convex polygon.
	 */
	~ConvexPolygon();

	/*!
	 * Compares two convex polygons for whether they cover the same area.
	 *
	 * Both convex polygons are assumed to be convex and minimal (as in, they
	 * don't have any colinear vertices).
	 *
	 * This equality check is regardless of the rotation of the vertices in the
	 * vertex list. Two convex polygons are equal even if their loops of
	 * vertices start with a different vertex.
	 */
	bool operator ==(const ConvexPolygon& other) const;

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