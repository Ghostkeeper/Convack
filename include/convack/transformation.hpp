/*
 * Library to pack convex polygons into arbitrary shapes.
 * Any copyright is dedicated to the public domain. See LICENSE.md for more details.
 */

#ifndef CONVACK_TRANSFORMATION
#define CONVACK_TRANSFORMATION

#include <array> //To store the transformation matrix data.

#include "coordinate.hpp" //For the translate function.

namespace convack {

class Point2;

/*!
 * This data structure represents a transformation matrix for 2D affine
 * transformations.
 */
class Transformation {
public:
	/*!
	 * Creates a new transformation matrix.
	 *
	 * The matrix will be initialised as an identity matrix.
	 */
	Transformation();

	/*!
	 * Apply this transformation to a point.
	 * \param point The point to apply the transformation to.
	 * \return A transformed point.
	 */
	Point2 apply(const Point2& point) const;

	/*!
	 * Adds a rotation to this transformation matrix.
	 *
	 * From this point on, the points transformed by this transformation will be
	 * rotated as well.
	 * \param angle_radians The angle to rotate the transformation by, in
	 * radians counter-clockwise.
	 * \return A reference to this transformation itself. This way you can chain
	 * multiple transformation functions.
	 */
	Transformation& rotate(const double angle_radians);

	/*!
	 * Adds a translation to this transformation matrix.
	 *
	 * From this point on, the points transformed by this transformation will be
	 * translated as well.
	 * \param x The distance to translate points in the X dimension.
	 * \param y The distance to translate points in the Y dimension.
	 * \return A reference to this transformation itself. This way you can chain
	 * multiple transformation functions.
	 */
	Transformation& translate(const coordinate_t x, const coordinate_t y);

private:
	/*!
	 * The cells of the transformation matrix.
	 *
	 * This is part of the 3x3 affine transformation matrix. Only the bottom row
	 * has been left out, because no transformation that we support will ever
	 * change that row to something else than `[0, 0, 1]`.
	 *
	 * The transformation matrix is written column-major, as is common with
	 * this sort of data.
	 */
	std::array<double, 6> data;
};

}

#endif