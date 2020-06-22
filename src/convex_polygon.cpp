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

	/*! @copydoc ConvexPolygon::contains(const Point2&) const
	 */
	bool contains(const Point2& point) const {
		if(vertices.size() < 3) {
			return false; //Even if it's on the one vertex or one edge, it's still not considered inside.
		}

		//TODO: Speed up by checking with a bounding box or reduced resolution first.

		//For each edge, check if the point is left of that edge. If it's not left for any of them, the point is outside.
		for(size_t i = 0; i < vertices.size(); ++i) {
			if(is_left(vertices[i], vertices[(i + 1) % vertices.size()], point) <= 0) {
				return false;
			}
		}
		return true;
	}

	/*! @copydoc ConvexPolygon::collides(const ConvexPolygon&) const
	 * \param check_other Whether to check collision from the other side. For a
	 * correct result, the collision check needs to happen from both sides. This
	 * parameter determines if we've already checked from the other side or not,
	 * to prevent going into an infinite loop of checking back and forth.
	 */
	bool collides(const Impl& other, const bool check_other = true) const {
		//TODO: Speed up by checking with a bounding box or reduced resolution first.

		//This uses the separating axes theorem (SAT) to find collisions between convex polygons in quadratic time.
		//Loop over all edges to see if the two convex polygons are completely separated by that axis.
		bool collision_this_side = true;
		for(size_t this_edge = 0; this_edge < vertices.size(); ++this_edge) {
			const Point2 edge_vector = vertices[(this_edge + 1) % vertices.size()] - vertices[this_edge];
			const Point2 axis_vector(-edge_vector.y, edge_vector.x); //Rotate 90 degrees to get an axis perpendicular to this edge to project the other polygon on.

			/* Because the convex hull is winding counterclockwise, this convex
			hull will always be completely to the left of the edge (negative on
			the projection axis). Therefore we only need to check the other
			convex polygon to see if it's completely to the right of the
			projection axis (weakly positive on the projection axis). If it's
			completely right, we've found the separating axis and the two are
			not colliding. */

			bool axis_overlap = false;
			const std::vector<Point2>& other_vertices = other.get_vertices(); //Convenience alias.
			for(size_t other_vertex = 0; other_vertex < other_vertices.size(); ++other_vertex) {
				const area_t projection = axis_vector.dot(other_vertices[other_vertex]);
				if(projection < 0) {
					axis_overlap = true;
					break;
				}
			}
			if(!axis_overlap) { //No overlap on this axis, so the two polygons don't collide.
				collision_this_side = false;
			}
		}
		if(collision_this_side) {
			return true; //None of the edges are separating, so the other polygon must be going through this one.
		}

		//Also check from the other side.
		if(check_other) {
			constexpr bool check_other_again = false; //Don't check this one again after checking the other. That would result in infinite recursion.
			return other.collides(*this, check_other_again);
		}
		return false; //Neither are colliding.
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
				const area_t left = is_left(last, best, next);
				if(left < 0) { //Is to the right.
					best = next;
				} else if(left == 0) { //Vertices are colinear.
					if((next - last).magnitude2() > (best - last).magnitude2()) { //Choose the vertex that is furthest away. Skip over vertices that are in between, to get a more efficient convex hull.
						best = next;
					}
				}
			}

			last = best;
		} while(last != result[0]); //Repeat until we automatically close the loop.

		return result;
	}

	/*!
	 * Tests whether the `query` point is to the left, to the right or on top of
	 * the line through `a` and `b`.
	 *
	 * This is a helper function to make a number of calculations easier.
	 *
	 * The result is a positive number if the `query` point is to the left of
	 * the line through `a` and `b`, a negative number if it's to the right, or
	 * zero when it's exactly on top of that line.
	 */
	static area_t is_left(const Point2& a, const Point2& b, const Point2& query) {
		return (static_cast<area_t>(b.x) - a.x) * (query.y - a.y) - (b.y - a.y) * (query.x - a.x);
	}
};

//Implementations of the parent ConvexPolygon class all refers on to the PIMPL class.

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

bool ConvexPolygon::contains(const Point2& point) const {
	return pimpl->contains(point);
}

bool ConvexPolygon::collides(const ConvexPolygon& other) const {
	return pimpl->collides(*other.pimpl);
}

const std::vector<Point2>& ConvexPolygon::get_vertices() const {
	return pimpl->get_vertices();
}

}