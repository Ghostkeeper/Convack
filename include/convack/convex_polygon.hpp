/*
 * Library to pack convex polygons into arbitrary shapes.
 * Any copyright is dedicated to the public domain. See LICENSE.md for more details.
 */

#ifndef CONVACK_CONVEX_POLYGON
#define CONVACK_CONVEX_POLYGON

#include <memory> //For unique_ptr.
#include <vector> //To store the coordinates of this convex polygon.

#include "area.hpp" //To compute cross products, dot products and areas.
#include "coordinate.hpp" //To translate the convex polygon by coordinates.

namespace convack {

class Point2;
class Transformation;

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
	 * Constructs a new convex hull around a number of convex polygons.
	 * \param convex_polygons The convex polygons to construct a convex hull
	 * around.
	 * \return A new convex polygon around all of the given convex polygons.
	 */
	static ConvexPolygon convex_hull(const std::vector<ConvexPolygon>& convex_polygons);

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
	 * \param other The convex polygon to compare to.
	 * \return `true` if the two convex polygons are equal, or `false` if they
	 * are different.
	 */
	bool operator ==(const ConvexPolygon& other) const;

	/*!
	 * Compares two convex polygons for whether they cover a different area.
	 *
	 * Both convex polygons are assumed to be convex and minimal (as in, they
	 * don't have any colinear vertices).
	 *
	 * This equality check is regardless of the rotation of the vertices in the
	 * vertex list. Two convex polygons are equal even if their loops of
	 * vertices start with a different vertex.
	 * \param other The convex polygon to compare to.
	 * \return `true` if the two convex polygons are different, or `false` if
	 * they are equal.
	 */
	bool operator !=(const ConvexPolygon& other) const;

	/*!
	 * Overloads streaming this convex polygon.
	 *
	 * This is useful for debugging, since it allows printing the convex polygon
	 * to a stream directly, giving you a reasonably readable output.
	 */
	friend std::ostream& operator <<(std::ostream& output_stream, const ConvexPolygon& convex_polygon);

	/*!
	 * Compute the surface area of this convex polygon.
	 * \return The surface area of this convex polygon.
	 */
	area_t area() const;

	/*!
	 * Tests whether the given point is contained in this convex polygon.
	 *
	 * Points that are exactly on the edge are not considered to be inside the
	 * convex polygon.
	 * \param point The point to test.
	 * \return `true` if that point is inside the polygon, or `false` if it
	 * isn't or on the boundary.
	 */
	bool contains(const Point2& point) const;

	/*!
	 * Test whether this convex polygon collides with another convex polygon.
	 *
	 * If it's only the vertices and edges of the convex polygon that collide,
	 * they are not considered to be colliding. The convex polygons are only
	 * considered to be colliding if they have an area of overlap greater than
	 * zero.
	 * \param other The convex polygon to check for collision against.
	 * \return `true` if this and the other convex polygon collide, or `false`
	 * if they don't.
	 */
	bool collides(const ConvexPolygon& other) const;

	/*!
	 * Get the current transformation of this convex polygon.
	 *
	 * This is the product of all transformations applied to the convex polygon
	 * since it was constructed. The original vertices can't be modified while
	 * the convex polygon instance is alive, so applying the inverse of this
	 * transformation matrix to the current vertex list should result in the
	 * original vertices that the convex polygon was constructed with.
	 * \return The transformation applied to the convex polygon that resulted in
	 * the current position of the vertices.
	 */
	const Transformation& current_transformation() const;

	/*!
	 * Get the vertices of the convex hull.
	 */
	const std::vector<Point2>& get_vertices() const;

	/*!
	 * Rotate this convex polygon.
	 *
	 * The rotation is always made around the coordinate origin `(0, 0)`. If the
	 * rotation needs to be made around a different point, translate the convex
	 * polygon first so that the translated rotation point ends up at `(0, 0)`,
	 * then rotate it, and then translate it back.
	 *
	 * The amount of rotation is in radians (not degrees) and counter-clockwise.
	 * \param angle_radians The amount of rotation in radians.
	 */
	void rotate(const double angle_radians);

	/*!
	 * Move this convex polygon in any direction.
	 * \param x The translation along the X axis.
	 * \param y The translation along the Y axis.
	 */
	void translate(const coordinate_t x, const coordinate_t y);

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