//Library to pack convex polygons into arbitrary shapes.
//Any copyright is dedicated to the public domain. See LICENSE.md for more details.

#include "convex_polygon.hpp" //The definitions of the implementation defined here.

namespace convack {

class ConvexPolygon::Impl {
public:
	/*! @copydoc ConvexPolygon::ConvexPolygon(const std::vector<Point2>&)
	 */
	Impl(const std::vector<Point2>& vertices) : vertices(vertices) {} //Copy the input vertices into this class.

	/*!
	 * The coordinates of the convex polygon.
	 */
	std::vector<Point2> vertices;

	/*! @copydoc ConvexPolygon::get_vertices() const
	 */
	const std::vector<Point2>& get_vertices() const {
		return vertices;
	}
};

ConvexPolygon::ConvexPolygon(const std::vector<Point2>& vertices) : pimpl(new Impl(vertices)) {}

const std::vector<Point2>& ConvexPolygon::get_vertices() const {
	return pimpl->get_vertices();
}

}