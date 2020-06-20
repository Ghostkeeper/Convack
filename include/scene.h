//Library to pack convex polygons into arbitrary shapes.
//Any copyright is dedicated to the public domain. See LICENSE.md for more details.

#ifndef CONVACK_SCENE
#define CONVACK_SCENE
namespace convack {

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
}

}
#endif
