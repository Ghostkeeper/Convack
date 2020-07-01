/*
 * Library to pack convex polygons into arbitrary shapes.
 * Any copyright is dedicated to the public domain. See LICENSE.md for more details.
 */

#include <algorithm> //For min_element.

#include "convex_polygon.hpp" //The definitions of the implementation defined here.
#include "point2.hpp" //To store the vertices of the convex polygon.
#include "transformation.hpp" //To translate and rotate the convex hull.

namespace convack {

class ConvexPolygon::Impl {
public:
	/*!
	 * The coordinates of the convex polygon.
	 */
	std::vector<Point2> vertices;

	/*!
	 * The transformation applied so far since the construction of the convex
	 * polygon.
	 *
	 * This transformation is already applied to the vertices. Don't apply it
	 * again. This field is only for bookkeeping. It's necessary to report a
	 * usable result to the user of the library if he wants to know how the
	 * convex polygons have been transformed to pack them.
	 */
	Transformation transformation;

	/*! @copydoc ConvexPolygon::convex_hull(const std::vector<Point2>&)
	 */
	static ConvexPolygon convex_hull(const std::vector<Point2>& points) {
		return gift_wrapping(points); //TODO: Use Chan's algorithm to construct convex hulls more efficiently.
	}

	/*! @copydoc ConvexPolygon::convex_hull(const std::vector<ConvexPolygon>&)
	 */
	static ConvexPolygon convex_hull(const std::vector<ConvexPolygon>& convex_polygons) {
		return chans_algorithm(convex_polygons);
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

	/*! @copydoc ConvexPolygon::area() const
	 */
	area_t area() const {
		/* This uses the shoelace formula to compute the area. The shoelace
		formula sums the areas of the individual triangles formed by two
		adjacent vertices and the coordinate origin.
		To calculate the area of a triangle with one vertex on the origin, we'll
		calculate the area of the parallelogram formed by the original triangle
		and that triangle mirrored around the line segment we're calculating the
		area for.
		The area of the parallelogram needs to be divided by two to arrive at
		the area of the triangle. Summing up all of those triangles (which may
		be negative if the edge is clockwise w.r.t. the coordinate origin)
		results in the total area of the convex polygon. This is the shoelace
		formula.*/
		area_t area = 0;
		for(size_t vertex = 0, previous = vertices.size() - 1; vertex < vertices.size(); ++vertex) {
			area += static_cast<area_t>(vertices[previous].x) * vertices[vertex].y - static_cast<area_t>(vertices[previous].y) * vertices[vertex].x;
			previous = vertex;
		}
		return area / 2; //Instead of dividing each parallelogram's area by 2, simply divide the total by 2 afterwards.
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
		if(vertices.size() < 3 || other.vertices.size() < 3) { //Either of the convex polygons has no area, so there can be no area of overlap.
			return false;
		}
		//TODO: Speed up by checking with a bounding box or reduced resolution first.

		//This uses the separating axes theorem (SAT) to find collisions between convex polygons in quadratic time.
		//Loop over all edges to see if the two convex polygons are completely separated by that axis.
		for(size_t this_edge = 0; this_edge < vertices.size(); ++this_edge) {
			const Point2 edge_vector = vertices[(this_edge + 1) % vertices.size()] - vertices[this_edge];
			const Point2 axis_vector(edge_vector.y, -edge_vector.x); //Rotate 90 degrees to get an axis perpendicular to this edge to project the other polygon on.

			/* Because the convex hull is winding counterclockwise, this convex
			hull will always be completely to the left of the edge (negative on
			the projection axis). Therefore we only need to check the other
			convex polygon to see if it's completely to the right of the
			projection axis (weakly positive on the projection axis). If it's
			completely right, we've found the separating axis and the two are
			not colliding. */

			bool axis_overlap = false;
			for(const Point2& other_vertex : other.vertices) {
				const area_t projection = axis_vector.dot(other_vertex - vertices[this_edge]);
				if(projection < 0) {
					axis_overlap = true;
					break;
				}
			}
			if(!axis_overlap) { //No overlap on this axis, so the two polygons don't collide.
				return false;
			}
		}

		//Also check from the other side.
		if(check_other) {
			constexpr bool check_other_again = false; //Don't check this one again after checking the other. That would result in infinite recursion.
			return other.collides(*this, check_other_again);
		}
		return true; //There is overlap everywhere, from both sides.
	}

	/*! @copydoc ConvexPolygon::current_transformation() const
	 */
	const Transformation& current_transformation() const {
		return transformation;
	}

	/*! @copydoc ConvexPolygon::get_vertices() const
	 */
	const std::vector<Point2>& get_vertices() const {
		return vertices;
	}

	/*! @copydoc ConvexPolygon::translate(const coordinate_t, const coordinate_t)
	 */
	void translate(const coordinate_t x, const coordinate_t y) {
		/* This actually applies the transformation to the vertices. We assume
		here that the vertices are requested more often than the transformation
		changed. Applying the transformation once is then more efficient. */
		const Transformation translation = Transformation().translate(x, y);
		for(Point2& vertex : vertices) {
			vertex = translation.apply(vertex);
		}
		transformation.translate(x, y); //Also track the transformation so far.
	}

	/*! @copydoc ConvexPolygon::rotate(const double)
	 */
	void rotate(const double angle_radians) {
		/* This actually applies the transformation to the vertices. We assume
		here that the vertices are requested more often than the transformation
		changed. Applying the transformation once is then more efficient. */
		const Transformation rotation = Transformation().rotate(angle_radians);
		for(Point2& vertex : vertices) {
			vertex = rotation.apply(vertex);
		}
		transformation.rotate(angle_radians); //Also track the transformation so far.
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
			return a.x < b.x || (a.x == b.x && a.y < b.y); //Select vertex most to negative X as starting point for the loop. The leftmost point is always in the convex hull.
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
	 * This is an implementation of the second stage of Chan's Algorithm, which
	 * creates a convex hull around a set of convex polygons.
	 *
	 * Chan's algorithm consists of two stages. First, the set of points to
	 * create a convex hull around is subdivided into a number of subsets and
	 * the convex hull is computed for each of those subsets. These convex hulls
	 * can be computed much faster since the subsets are smaller (especially if,
	 * like in this library, the ordinary convex hull algorithm is quadratic).
	 * And secondly, it computes the convex hull around each of those convex
	 * polygons using properties of the convex hull to speed that up.
	 *
	 * In the second stage of Chan's algorithm, implemented here, a new convex
	 * hull is constructed using a variant of the gift wrapping algorithm that
	 * makes use of the properties of the convex hull to speed up the algorithm.
	 * As it iteratively constructs the vertices of the resulting convex hull,
	 * it searches the right-most vertex of each convex polygon w.r.t. the
	 * direction of the last completed edge using a binary search. Of all of
	 * these right-most vertices, it chooses the one that's the right-most of
	 * them all as the new vertex of the final convex hull. This repeats until
	 * the first vertex is obtained again and the convex hull is closed.
	 * \param convex_polygons A set of polygons to create a convex hull around.
	 * \return A convex hull around all of the polygons.
	 */
	static ConvexPolygon chans_algorithm(const std::vector<ConvexPolygon>& convex_polygons) {
		if(convex_polygons.empty()) {
			return ConvexPolygon({});
		}
		if(convex_polygons.size() == 1) {
			return convex_polygons[0];
		}

		//First find the left-most vertex among all convex polygons.
		//This vertex is always in the convex hull.
		Point2 best(std::numeric_limits<coordinate_t>::max(), 0);
		size_t best_vertex = -1;
		size_t best_polygon = -1;
		for(size_t polygon = 0; polygon < convex_polygons.size(); ++polygon) {
			const std::vector<Point2>& vertices = convex_polygons[polygon].get_vertices();
			if(vertices.empty()) {
				continue;
			}
			//Perform a binary search to find the left-most vertex of this convex polygon.
			size_t lower_bound = 0;
			size_t upper_bound = vertices.size();
			if(vertices.size() > 1 && //If there's just 1 vertex, lower_bound is already the left-most vertex.
				!((vertices[0].x < vertices[1].x || (vertices[0].x == vertices[1].x && vertices[0].y < vertices[1].y)) //Test if vertex 0 is the left-most vertex! The binary search below doesn't take vertices[0] into account.
				&& (vertices[0].x < vertices.back().x || (vertices[0].x == vertices.back().x && vertices[0].y < vertices.back().y)))
			) {
				while(upper_bound - lower_bound > 1) { //If the bounds are just 1 vertex apart, we've found our answer.
					const size_t pivot = (upper_bound + lower_bound) / 2;
					const Point2& pivot_point = vertices[pivot];
					//Find on which side of the pivot the left-most vertex is.
					//For an intuition, see the diagrams and explanation at http://geomalgorithms.com/a14-_extreme_pts.html
					const Point2& pivot_next = vertices[(pivot + 1) % vertices.size()];
					const bool pivot_goes_left = pivot_next.x < pivot_point.x || (pivot_next.x == pivot_point.x && pivot_next.y < pivot_point.y);
					const Point2& pivot_previous = vertices[(pivot - 1 + vertices.size()) % vertices.size()];
					if(!pivot_goes_left && (pivot_point.x < pivot_previous.x || (pivot_point.x == pivot_previous.x && pivot_point.y < pivot_previous.y))) { //This is a local optimum. We're done!
						lower_bound = pivot;
						break;
					}

					const Point2& lower_point = vertices[lower_bound];
					const Point2& lower_next = vertices[(lower_bound + 1) % vertices.size()];
					const bool lower_goes_left = lower_next.x < lower_point.x || (lower_next.x == lower_point.x && lower_next.y < lower_point.y);

					if(lower_goes_left) {
						if(!pivot_goes_left) {
							//Beyond the lower bound, but not beyond the pivot point, so it must be in the lower range.
							upper_bound = pivot;
						} else {
							//Beyond both the lower bound and the pivot point. That means it's after whichever one is most to the left.
							if(pivot_point.x < lower_point.x || (pivot_point.x == lower_point.x && pivot_point.y < lower_point.y)) {
								lower_bound = pivot + 1;
							} else {
								upper_bound = pivot;
							}
						}
					} else {
						if(pivot_goes_left) {
							//Before the lower bound, but after the pivot point, so it must be in the upper range.
							lower_bound = pivot + 1;
						} else {
							//Before both the lower bound and the pivot point. That means it's beyond whichever one is most to the right.
							if(pivot_point.x > lower_point.x || (pivot_point.x == lower_point.x && pivot_point.y > lower_point.y)) {
								lower_bound = pivot + 1;
							} else {
								upper_bound = pivot;
							}
						}
					}
				}
			}
			if(vertices[lower_bound].x < best.x || (vertices[lower_bound].x == best.x && vertices[lower_bound].y < best.y)) {
				best = vertices[lower_bound];
				best_vertex = lower_bound;
				best_polygon = polygon;
			}
		}
		if(best_polygon == size_t(-1)) { //All convex polygons were empty.
			return ConvexPolygon({});
		}
		std::vector<Point2> result;

		//Now iteratively find the next point on the resulting polygon by choosing the right-most convex hull.
		Point2 last = best;
		size_t last_polygon = best_polygon;
		size_t last_vertex = best_vertex;
		do {
			result.push_back(best);

			//For the polygon on which the last vertex lays, we already know which the right-most vertex is: It's the next one in the chain.
			best_vertex = (last_vertex + 1) % convex_polygons[last_polygon].get_vertices().size();
			best = convex_polygons[last_polygon].get_vertices()[best_vertex];
			//Scan through the other convex polygons if we find anything better.
			for(size_t polygon = (last_polygon + 1) % convex_polygons.size(); polygon != last_polygon; polygon = (polygon + 1) % convex_polygons.size()) {
				const std::vector<Point2>& vertices = convex_polygons[polygon].get_vertices();
				if(vertices.empty()) {
					continue;
				}
				//Perform a binary search to find the right-most vertex of this convex polygon (compared to the edge between the last and current best vertices).
				//N.B. This is not the greatest X coordinate, but rather the greatest negative angle!
				size_t lower_bound = 0;
				size_t upper_bound = vertices.size();
				if(vertices.size() > 1) { //If there's just 1 vertex, lower_bound is already the right-most vertex.
					//Test if vertices[0] is already the right-most vertex! The binary search below doesn't take vertices[0] into account.
					const area_t vertex0_left_of_1 = is_left(last, vertices[1], vertices[0]);
					const area_t vertex0_left_of_b = is_left(last, vertices.back(), vertices[0]);
					if(!((vertex0_left_of_1 < 0 || (vertex0_left_of_1 == 0 && (vertices[0] - last).magnitude2() > (vertices[1] - last).magnitude2()))
					  && (vertex0_left_of_b < 0 || (vertex0_left_of_b == 0 && (vertices[0] - last).magnitude2() > (vertices.back() - last).magnitude2())))) {
						while(upper_bound - lower_bound > 1) { //If the bounds are just 1 vertex apart, we've found our answer.
							const size_t pivot = (upper_bound + lower_bound) / 2;
							const Point2& pivot_point = vertices[pivot];
							const Point2& pivot_next = vertices[(pivot + 1) % vertices.size()];
							
							
							const area_t pivot_how_left = is_left(last, pivot_point, pivot_next);
							const bool pivot_goes_right = pivot_how_left < 0 || (pivot_how_left == 0 && (pivot_next - last).magnitude2() > (pivot_point - last).magnitude2());

							if(!pivot_goes_right) {
								//Calculate for the previous point to see if the pivot is already the extremum by chance.
								const Point2& pivot_previous = vertices[(pivot - 1 + vertices.size()) % vertices.size()];
								const area_t pivot_previous_how_left = is_left(last, pivot_previous, pivot_point);
								if(pivot_previous_how_left < 0 || (pivot_previous_how_left == 0 && (pivot_point - last).magnitude2() > (pivot_previous - last).magnitude2())) {
									//Pivot goes left, previous goes right towards the pivot. Then pivot is the extremum!
									lower_bound = pivot;
									break;
								}
							}

							const Point2& lower_point = vertices[lower_bound];
							const Point2& lower_next = vertices[(lower_bound + 1) % vertices.size()];
							const area_t lower_how_left = is_left(last, lower_point, lower_next);
							const bool lower_goes_right = lower_how_left < 0 || (lower_how_left == 0 && (lower_next - last).magnitude2() > (lower_point - last).magnitude2());

							//Find on which side of the pivot the right-most vertex is.
							//For an intuition, see the diagrams and explanation at http://geomalgorithms.com/a14-_extreme_pts.html
							if(lower_goes_right) {
								if(!pivot_goes_right) {
									//Beyond the lower bound, but not beyond the pivot point, so it must be in the lower range.
									upper_bound = pivot;
								} else {
									//Beyond both the lower bound and the pivot point. That means it's after whichever one is most to the right.
									if(pivot_how_left < lower_how_left || (pivot_how_left == lower_how_left && (pivot_point - last).magnitude2() > (lower_point - last).magnitude2())) {
										lower_bound = pivot + 1;
									} else {
										upper_bound = pivot;
									}
								}
							} else {
								if(pivot_goes_right) {
									//Before the lower bound, but after the pivot point, so it must be in the upper range.
									lower_bound = pivot + 1;
								} else {
									//Before both the lower bound and the pivot point. That means it's beyond whichever one is most to the right.
									if(pivot_how_left > lower_how_left || (pivot_how_left == lower_how_left && (lower_point - last).magnitude2() > (pivot_point - last).magnitude2())) {
										lower_bound = pivot + 1;
									} else {
										upper_bound = pivot;
									}
								}
							}
						}
					}
				}

				//We've found the right-most vertex of this convex polygon (index lower_bound). Is it better than the current best?
				const area_t how_left = is_left(last, best, vertices[lower_bound]);
				if(how_left < 0 || (how_left == 0 && (vertices[lower_bound] - last).magnitude2() > (best - last).magnitude2())) { //Is it more to the right, or equally but farther?
					best = vertices[lower_bound];
					best_vertex = lower_bound;
					best_polygon = polygon;
				}
			}
			//The best vertex is now the right-most vertex of all convex hulls!

			last = best;
			last_polygon = best_polygon;
			last_vertex = best_vertex;
		} while(best != result[0]); //Continue until we're looping back to the first vertex of the result.

		return ConvexPolygon(result);
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

ConvexPolygon ConvexPolygon::convex_hull(const std::vector<ConvexPolygon>& convex_polygons) {
	return ConvexPolygon::Impl::convex_hull(convex_polygons);
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

area_t ConvexPolygon::area() const {
	return pimpl->area();
}

bool ConvexPolygon::contains(const Point2& point) const {
	return pimpl->contains(point);
}

bool ConvexPolygon::collides(const ConvexPolygon& other) const {
	return pimpl->collides(*other.pimpl);
}

const Transformation& ConvexPolygon::current_transformation() const {
	return pimpl->current_transformation();
}

const std::vector<Point2>& ConvexPolygon::get_vertices() const {
	return pimpl->get_vertices();
}

ConvexPolygon& ConvexPolygon::translate(const coordinate_t x, const coordinate_t y) {
	pimpl->translate(x, y);
	return *this;
}

ConvexPolygon& ConvexPolygon::rotate(const double angle_radians) {
	pimpl->rotate(angle_radians);
	return *this;
}

}