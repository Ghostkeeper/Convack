/*
 * Library to pack convex polygons into arbitrary shapes.
 * Any copyright is dedicated to the public domain. See LICENSE.md for more details.
 */

#include <algorithm> //For min_element.

#include "area.hpp" //To compute cross products, dot products and areas.
#include "convex_polygon.hpp" //The definitions of the implementation defined here.

namespace convack {

class ConvexPolygon::Impl {
public:
	/*!
	 * The coordinates of the convex polygon.
	 */
	std::vector<Point2> vertices;

	/*! @copydoc ConvexPolygon::convex_hull(const std::vector<Point2>&)
	 */
	static ConvexPolygon convex_hull(const std::vector<Point2>& points) {
		return gift_wrapping(points); //TODO: Use Chan's algorithm to construct convex hulls more efficiently.
	}

	/*! @copydoc ConvexPolygon::ConvexPolygon(const std::vector<Point2>&)
	 */
	Impl(const std::vector<Point2>& vertices) : vertices(vertices) {} //Copy the input vertices into this class.

	/*! @copydoc ConvexPolygon::operator ==(const ConvexPolygon&) const
	 */
	bool operator ==(const ConvexPolygon& other) const {
		const std::vector<Point2>& other_vertices = other.get_vertices();
		if(vertices.size() != other_vertices.size()) {
			return false; //Early out for performance. This is easy to check.
		}
		if(vertices.empty()) { //Both have size 0, and are thus equal.
			return true;
		}

		for(size_t offset = 0; offset < vertices.size(); ++offset) { //Try with any rotation of the vertices.
			bool match = true;
			for(size_t i = 0; i < vertices.size(); ++i) {
				if(vertices[i] != other_vertices[(i + offset) % vertices.size()]) {
					match = false;
					break;
				}
			}
			if(match) {
				return true;
			}
		}
		return false; //No match with any rotation.
	}

	/*! @copydoc ConvexPolygon::operator !=(const ConvexPolygon&) const
	 */
	bool operator !=(const ConvexPolygon& other) const {
		return !(*this == other);
	}

	/*! @copydoc ConvexPolygon::operator <<(std::ostream&, const ConvexPolygon&)
	 */
	friend std::ostream& operator <<(std::ostream& output_stream, const Impl& convex_polygon_impl) {
		output_stream << "[";
		for(size_t i = 0; i < convex_polygon_impl.vertices.size(); ++i) {
			if(i != 0) {
				output_stream << ", ";
			}
			if(i >= 32) { //Output at most 32 vertices, to prevent spamming the output stream for debugging purposes.
				return output_stream << "...]";
			}
			output_stream << convex_polygon_impl.vertices[i];
		}
		return output_stream << "]";
	}

	/*! @copydoc ConvexPolygon::get_vertices() const
	 */
	const std::vector<Point2>& get_vertices() const {
		return vertices;
	}

private:
	/*!
	 * Executes the gift wrapping algorithm on a set of points to create a
	 * convex hull around them.
	 */
	static ConvexPolygon gift_wrapping(const std::vector<Point2>& points) {
		if(points.size() <= 2) { //Though a triangle (3 vertices) is always convex, don't immediately return it since it could have incorrect winding.
			return ConvexPolygon(points);
		}

		std::vector<Point2> result;
		Point2 last = *std::min_element(points.begin(), points.end(), [](const Point2& a, const Point2& b) {
			return a.x < b.x; //Select vertex most to negative X as starting point for the loop. The leftmost point is always in the convex hull.
		});

		do {
			result.push_back(last);

			//Find the right-most vertex.
			//Here we take two points: The best candidate so far, and any new vertex to consider.
			//The new vertex is better than the best candidate if it's to the right of the line from the last convex hull vertex to the previous best.

			//Start off with a candidate that is NOT equal to the best (many vertices may be overlapping, so loop until we've found one).
			Point2 best = last;
			for(Point2 candidate : points) {
				if(candidate != last) {
					best = candidate;
					break;
				}
			}
			
			//Now update the `best` vertex if we find a new vertex that is to the right of the line from `last` to `best`.
			for(Point2 next : points) {
				const area_t determinant = (static_cast<area_t>(best.x) - last.x) * (next.y - last.y) - (best.y - last.y) * (next.x - last.x);
				if(determinant < 0) {
					best = next;
				} else if(determinant == 0) { //Vertices are colinear.
					if((next - last).magnitude2() > (best - last).magnitude2()) { //Choose the vertex that is furthest away. Skip over vertices that are in between, to get a more efficient convex hull.
						best = next;
					}
				}
			}

			last = best;
		} while(last != result[0]); //Repeat until we automatically close the loop.

		return result;
	}
};

ConvexPolygon ConvexPolygon::convex_hull(const std::vector<Point2>& points) {
	return ConvexPolygon::Impl::convex_hull(points);
}

ConvexPolygon::ConvexPolygon(const std::vector<Point2>& vertices) : pimpl(new Impl(vertices)) {}

ConvexPolygon::ConvexPolygon(const ConvexPolygon& original) : pimpl(new Impl(*original.pimpl)) {}

ConvexPolygon::~ConvexPolygon() = default; //Defined here where there is a complete type for Impl, so that the unique_ptr can be deleted.

bool ConvexPolygon::operator ==(const ConvexPolygon& other) const {
	return *pimpl == other;
}

bool ConvexPolygon::operator !=(const ConvexPolygon& other) const {
	return *pimpl != other;
}

std::ostream& operator <<(std::ostream& output_stream, const ConvexPolygon& convex_polygon) {
	return output_stream << *convex_polygon.pimpl;
}

const std::vector<Point2>& ConvexPolygon::get_vertices() const {
	return pimpl->get_vertices();
}

}