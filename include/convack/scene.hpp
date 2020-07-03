/*
 * Library to pack convex polygons into arbitrary shapes.
 * Any copyright is dedicated to the public domain. See LICENSE.md for more details.
 */

#ifndef CONVACK_SCENE
#define CONVACK_SCENE

#include <memory> //For unique_ptr.
#include <vector> //As input for a set of convex polygons.

namespace convack {

class ConvexPolygon;

/*!
 * A space to pack convex polygons into.
 *
 * The purpose of this object is that you can add convex polygons to it, and
 * then instruct the scene to pack them, which gives a series of transformation
 * matrices that would translate/rotate them to their packed locations.
 *
 * Optionally the scene has its own enclosed space, which is also a convex
 * polygon inside which the convex polygons must be packed. The scene can also
 * have obstructions that cannot be moved, around which other convex polygons
 * must be packed.
 *
 * Global options for packing, such as settings for how many combinations it may
 * search, can be configured separately for each scene as well. They are stored
 * in this scene.
 */
class Scene {
public:
	/*!
	 * Constructs an empty scene.
	 *
	 * The scene has no enclosed space or obstructions, and no polygons are
	 * packed into the scene yet.
	 */
	Scene();

	/*!
	 * Create a packing of a given list of convex polygons.
	 *
	 * The convex polygons will be modified in-place to fit in the scene. To
	 * draw the result, you can use the modified vertex lists in the convex
	 * polygons. To figure out how the convex polygons were moved and/or rotated
	 * in order to pack them, refer to the transformation of the convex
	 * polygons.
	 *
	 * The order of these convex polygons will not be modified. You can use this
	 * order to identify which convex polygon was which.
	 * \param convex_polygons A list of convex polygons that need to be packed.
	 * This list only needs to exist during the execution of the packing
	 * algorithm. The packed convex polygons don't stay in the scene. The result
	 * will be stored in this same list.
	 */
	void pack(std::vector<ConvexPolygon>& convex_polygons) const;

private:
	/*!
	 * The implementation of the scene is separated into this class.
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