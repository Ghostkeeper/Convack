Convack is intended to become a library to perform polygon packing with convex polygons.

This library was once developed as an option to try and see how difficult this problem is in practice. The idea was that it might replace the arrange algorithm in [Cura](https://github.com/Ultimaker/Cura) at some point. However with Cura we've chosen a different library instead, as indeed the problem can become quite complex.

Ever since the need for this library is removed by Cura, I've not developed it any further. It served its use as a basic set-up for geometry algorithms and a nice convex hull implementation. But beyond that there is hardly anything useful in this repository.

Objective
----
The origin of this project is the need for printed object packing in [Cura](https://github.com/Ultimaker/Cura). From there, the requirements of this project are at least:
* Only needs to be able to handle packing convex polygons (concave polygons are optional).
* Needs to be able to handle <50 polygons with decent resolution (let's say <300 vertices each) at reasonable speed, within a few tenths of a second.
* Needs to handle both rotation and translation.
* Needs to be able to pack polygons with un-movable barriers in the scene.
* Needs to be able to be published in an LGPL-licensed project.

At the time of writing, no other project has met all of these demands. However recently [libnest2d](https://github.com/tamasmeszaros/libnest2d) changed their license and then meets all of those requirements.

Implementation
----
The intended implementation for this project is a beam search.

Imagine a tree structure (since I don't feel like drawing a diagram right now) that enumerates every possible permutation of the N objects that need to be added to the scene in its leaves. At the root there is a node with 0 inserted objects, and it has N children to decide which object to insert in the scene first. It will compute a fitness for each of these nodes, decides on the `K` best solutions among them, and expand those nodes with a second inserted object. Again it chooses the `K` best solutions among all of the current leaves (which may have either 1 or 2 objects at that point, as the previous layer is still included) and explores the `N-2` children of those, and so on. Once a sufficient number of nodes has reached the maximum height of the tree, where no more objects are left to be inserted, the algorithm will terminate and choose the node with the greatest fitness as the winner.

This is as of this writing all still theory. Other implementations are also possible if this fails. I'm still dealing with the basis of this, which includes collision checking between convex polygons and measuring the fitness of a packing in some way.
