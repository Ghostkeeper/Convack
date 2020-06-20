//Library to pack convex polygons into arbitrary shapes.
//Any copyright is dedicated to the public domain. See LICENSE.md for more details.

#ifndef CONVACK_CONVEX_POLYGON
#define CONVACK_CONVEX_POLYGON

namespace convack {

/*!
 * Data structure representing one convex polygon.
 *
 * The class by itself doesn't enforce that it's convex, but does assume that it
 * is. It's the duty of the consumer of this class to ensure that it is convex.
 * Alternatively, it also provides a method to create a convex polygon enclosing
 * a number of given coordinates.
 *
 * This class contains a number of algorithms operating on convex polygons. Due
 * to the nature of the convexity constraint, many algorithms become
 * considerably simpler and faster when working only with convex polygons.
 */
class ConvexPolygon {
};

}

#endif