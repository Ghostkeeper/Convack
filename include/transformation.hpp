/*
 * Library to pack convex polygons into arbitrary shapes.
 * Any copyright is dedicated to the public domain. See LICENSE.md for more details.
 */

#ifndef CONVACK_TRANSFORMATION
#define CONVACK_TRANSFORMATION

#include <array> //To store the transformation matrix data.

namespace convack {

/*!
 * This data structure represents a transformation matrix for 2D affine
 * transformations.
 */
struct Transformation {
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

	/*!
	 * Creates a new transformation matrix.
	 *
	 * The matrix will be initialised as an identity matrix.
	 */
	Transformation();
};

}

#endif