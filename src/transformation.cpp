/*
 * Library to pack convex polygons into arbitrary shapes.
 * Any copyright is dedicated to the public domain. See LICENSE.md for more details.
 */

#include "transformation.hpp"

#include <cmath> //To compute rotation matrices.

namespace convack {

Transformation::Transformation() {
	//Initialise as an identity matrix:
	// 1 0 0
	// 0 1 0
	data = {1, 0, 0, 1, 0, 0};
}

Transformation& Transformation::translate(const coordinate_t x, const coordinate_t y) {
	data[4] += x;
	data[5] += y;
	return *this;
}

Transformation& Transformation::rotate(const double angle_radians) {
	const double cosine = std::cos(angle_radians);
	const double sine = std::sin(angle_radians);

	std::array<double, 6> new_data; //Use a temporary intermediary to prevent the order of calculations from influencing the result.
	new_data[0] = cosine * data[0] - sine * data[1];
	new_data[1] = sine * data[0] + cosine * data[1];
	new_data[2] = cosine * data[2] - sine * data[3];
	new_data[3] = sine * data[2] + cosine * data[3];
	new_data[4] = cosine * data[4] - sine * data[5];
	new_data[5] = sine * data[5] + cosine * data[6];
	std::swap(data, new_data);
	return *this;
}

}