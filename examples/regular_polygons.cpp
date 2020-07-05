/*
 * Library to pack convex polygons into arbitrary shapes.
 * Any copyright is dedicated to the public domain. See LICENSE.md for more details.
 */

/* This is a typical example of how to pack a number of convex polygons in a
scene and get the results. This example involves the following steps:
- Prepare a number of convex polygons to pack. In this case we're going to pack
  the regular polygons between 3 and 10 vertices.
- Instruct Convack to pack these polygons tightly around the 0,0 coordinate.
- Read out the resulting convex polygons.
- Write the result to an SVG file. */

#include <cmath> //To calculate the positions of the vertices for the regular polygons.
#include <convack/convex_polygon.hpp> //The convex polygons we're going to pack.
#include <convack/point2.hpp> //To generate convex polygons.
#include <convack/scene.hpp> //To bootstrap the packing action.
#include <iostream> //To show progress and errors.
#include <vector> //To create a list of regular polygons.

/*!
 * Generate the regular polygons that we're going to be packing.
 * \return A list of regular polygons to pack.
 */
std::vector<convack::ConvexPolygon> create_regular_polygons() {
	std::vector<convack::ConvexPolygon> result;
	constexpr float radius = 10;
	constexpr double pi = std::acos(-1);
	for(size_t num_sides = 3; num_sides <= 10; ++num_sides) {
		std::vector<convack::Point2> vertices;
		for(size_t i = 0; i < num_sides; ++i) {
			double angle = 2.0 * pi / num_sides * i;
			vertices.emplace_back(std::cos(angle) * radius, std::sin(angle) * radius);
		}
		result.emplace_back(vertices);
	}
	return result;
}

int main(int argc, char** argv) {
	std::cout << "Generating regular polygons..." << std::endl;
	std::vector<convack::ConvexPolygon> regular_polygons = create_regular_polygons(); //The polygons we want to pack.

	//Convack works by preparing a scene where you're going to pack the convex polygons in.
	//You can then change the packing settings, add obstacles to the scene, etc.
	//In this case we don't need to do all that and just want to pack a bunch of polygons together.
	std::cout << "Packing..." << std::endl;
	convack::Scene().pack(regular_polygons); //Perform the actual packing.

	std::cout << "Done!" << std::endl;
}